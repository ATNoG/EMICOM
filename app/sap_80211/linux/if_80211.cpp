//=============================================================================
// Brief   : Netlink interface for 802.11 devices
// Authors : André Prata <andreprata@av.it.pt>
//-----------------------------------------------------------------------------
// ODTONE - Open Dot Twenty One
//
// Copyright (C) 2009-2012 Universidade Aveiro
// Copyright (C) 2009-2012 Instituto de Telecomunicações - Pólo Aveiro
//
// This software is distributed under a license. The full license
// agreement can be found in the file LICENSE in this distribution.
// This software may not be copied, modified, sold or distributed
// other than expressed in the named license agreement.
//
// This software is distributed without any warranty.
//==============================================================================

#define MAC_ALEN 18

#include "if_80211.hpp"

#include <boost/thread.hpp>
#include <boost/scope_exit.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <odtone/mih/tlv_types.hpp>

#include <odtone/logger.hpp>
#include <iostream>
#include <stdexcept>

#define _LINUX_IF_H // workaround
#include <netlink/route/link.h>

using namespace odtone;

static logger log_("if_80211", std::cout);

///////////////////////////////////////////////////////////////////////////////

mih::link_dn_reason reason_code_2_dn_reason(unsigned short rc)
{
	switch (rc) {
		case 39:
			return mih::link_dn_reason(mih::link_dn_reason_packet_timeout);
			break;
		case 5:
		case 31:
		case 32:
		case 33:
		case 34:
		case 45:
			return mih::link_dn_reason(mih::link_dn_reason_no_resource);
			break;
		case 4:
		//case 39:
			return mih::link_dn_reason(mih::link_dn_reason_no_broadcast);
			break;
		case 2:
		case 6:
		case 7:
		case 9:
		case 23:
		case 24:
			return mih::link_dn_reason(mih::link_dn_reason_authentication_failure);
			break;
		default:
			break;
	}

	return mih::link_dn_reason(mih::link_dn_reason_explicit_disconnect);
}

//
//
// Commented code refers to country specific frequencies
//
unsigned int frequency_to_channel_id(unsigned int frequency)
{
	int ret = 0;

	if (frequency > 5000) {
		ret = (frequency - 5000) / 5;
//	} else if (frequency > 4000) {
//		ret = (frequency - 4000) / 5;
//	} else if (frequency > 3000) {
//		// not all of these are integer, and we are uncertain of
//		// how the kernel encodes those, but assume the integer
//		// part of such frequencies, the function works as well!
//		ret = (frequency - 3000) / 5
//
//		// special case
//		if (frequency == 3689) {
//			//(frequency - 3000) / 5 + 1;
//			ret = 138;
//		}
//	// another special case
//	} else if (frequency == 2484) {
//		ret = 14;
	} else if (frequency > 2400) {
		ret = (frequency - 2407) / 5;
	}

	return ret;
}

int signal_scan_finish_handler(::nl_msg *msg, void *arg)
{
	int *command = static_cast<int *>(arg);
	nlwrap::genl_msg m(msg);

	if (m.cmd() == NL80211_CMD_NEW_SCAN_RESULTS || m.cmd() == NL80211_CMD_SCAN_ABORTED) {
		*command = m.cmd();
	}

	return NL_SKIP;
}

///////////////////////////////////////////////////////////////////////////////

struct scan_results_data {
	std::vector<poa_info> l;

	boost::optional<odtone::uint> associated_index;
	if_80211::ctx_data &_ctx;

	scan_results_data(if_80211::ctx_data &ctx) : _ctx(ctx) {}
};

int handle_operstate(nl_msg *msg, void *arg)
{
	try {
		nlwrap::genl_msg m(msg);
		int *state = static_cast<int *>(arg);

		if (m.attr_ps_state) {
			*state = m.attr_ps_state.get();
		}
	} catch(...) {
		log_(0, "(command) Error parsing ps_state message");
	}

	return NL_SKIP;
}

int handle_scan_results(::nl_msg *msg, void *arg)
{
	try {
		nlwrap::genl_msg m(msg);
		scan_results_data *d = static_cast<scan_results_data*>(arg);

		if (m.cmd() != NL80211_CMD_NEW_SCAN_RESULTS && m.cmd() != NL80211_CMD_SCAN_ABORTED) {
			return NL_SKIP;
		}

		if (!(m.ie_ssid && m.bss_frequency && m.bss_bssid && m.bss_signal_mbm && m.ie_max_data_rate)) {
			log_(0, "(command) Not all required elements present in scan dump result");
			return NL_SKIP;
		}

		if (m.bss_status && m.bss_status.get() == NL80211_BSS_STATUS_ASSOCIATED) {
			d->associated_index = d->l.size();
		}

		mih::net_caps caps;
		if (m.ie_has_security_features) {
			caps.set(mih::net_caps_security);
		}
		if (m.bss_qos_capable) {
			caps.set(mih::net_caps_qos_0); // can't assume classes!
		}

		poa_info i;

		mih::mac_addr poa_addr = mih::mac_addr(m.bss_bssid.get().c_str());

		i.network_id = m.ie_ssid.get();
		i.channel_id = frequency_to_channel_id(m.bss_frequency.get());
		i.id.type = mih::link_type_802_11;
		i.id.addr = d->_ctx._mac;
		i.id.poa_addr = poa_addr;
		i.signal = m.bss_signal_mbm.get();
		i.data_rate = m.ie_max_data_rate.get() * 1000;
		i.mih_capabilities = mih::link_mihcap_flag();
		i.net_capabilities = caps;
		//i.sinr = 0;
		// SINR cannot be obtained, since we can only get the signal strength
		// and the noise value (through NL80211_CMD_GET_SURVEY). (missing "interference")

		d->l.push_back(i);

		// save the mac:ssid pair
		boost::unique_lock<boost::shared_mutex> lock(d->_ctx._seen_bssids_access);
		d->_ctx._seen_bssids[poa_addr.address()] = i;
	} catch(...) {
		log_(0, "(command) Error parsing scan dump message");
	}

	return NL_SKIP;
}

int handle_station_results_signal(::nl_msg *msg, void *arg)
{
	try {
		nlwrap::genl_msg m(msg);
		sint8 *d = static_cast<sint8 *>(arg);

		if (m.sta_info_signal) {
			*d = m.sta_info_signal.get();
		}
	} catch(...) {
		log_(0, "(command) Error parsing station dump message");
	}

	return NL_SKIP;
}

int handle_station_results_rate(::nl_msg *msg, void *arg)
{
	try {
		nlwrap::genl_msg m(msg);
		odtone::uint *d = static_cast<odtone::uint *>(arg);

		if (m.sta_rate_info_bitrate) {
			*d = m.sta_rate_info_bitrate.get();
		}
	} catch(...) {
		log_(0, "(command) Error parsing station dump message");
	}

	return NL_SKIP;
}

void fetch_scan_results(scan_results_data &data)
{
	log_(0, "(command) Dumping scan results");

	nlwrap::genl_socket s;

	nlwrap::genl_msg m(s.family_id("nl80211"), NL80211_CMD_GET_SCAN, NLM_F_DUMP);
	m.put_ifindex(data._ctx._ifindex);

	nlwrap::nl_cb cb(handle_scan_results, static_cast<void *>(&data));

	// uncommenting this prevents the "known bssid" table to grow indefinitely.
	// however, it poses concurrency issues, and access must be synchronized!
	//{boost::unique_lock<boost::shared_mutex> lock(data._ctx._seen_bssids_access);
	//data._ctx._seen_bssids.clear();}

	s.send(m);

	while (!cb.finish()) {
		s.receive(cb);
	}

	if (cb.error()) {
		throw std::runtime_error("Error, code: " + boost::lexical_cast<std::string>(cb.error_code()));
	}

	log_(0, "(command) Dumped ", data.l.size(), " scan results");
}

bool compare_poa_strength(const poa_info &a, const poa_info &b)
{
	const odtone::sint8 *a_dbm = boost::get<odtone::sint8>(&a.signal);
	const odtone::sint8 *b_dbm = boost::get<odtone::sint8>(&b.signal);

	if (a_dbm && b_dbm) {
		return *a_dbm > *b_dbm;
	} else {
		const mih::percentage *a_pct = boost::get<mih::percentage>(&a.signal);
		const mih::percentage *b_pct = boost::get<mih::percentage>(&b.signal);
		if (a_pct && b_pct) {
			odtone::uint _a = *((const odtone::uint *)a_pct);
			odtone::uint _b = *((const odtone::uint *)b_pct);
			return _a > _b;
		}
	}

	throw std::runtime_error("SIG_STRENGTH info not available for sorting");
}

void dispatch_strongest_scan_results(scan_results_data &d)
{
	// sort by strongest signal
	std::sort(d.l.begin(), d.l.end(), compare_poa_strength);

	std::map<mih::octet_string, bool> announced;
	if (d.associated_index) {
		announced[d.l[d.associated_index.get()].network_id] = true;
	}

	BOOST_FOREACH(poa_info &i, d.l) {
		if (!announced[i.network_id]) {
			announced[i.network_id] = true;
			d._ctx._ios.dispatch(boost::bind(d._ctx._detected_handler.get(), i));
		}
	}
}

int handle_nl_event(nl_msg *msg, void *arg)
{
	nlwrap::genl_msg m(msg);
	if_80211::ctx_data *ctx = static_cast<if_80211::ctx_data *>(arg);

	if(!m.attr_ifindex || m.attr_ifindex.get() != ctx->_ifindex) {
		return NL_SKIP;
	}

	// Parse the event
	switch (m.cmd()) {
	case NL80211_CMD_TRIGGER_SCAN:
		{
			// This event is caught just to set this variable,
			// and try to prevent overlapping ongoing scans
			log_(0, "(event) Scan started");
			ctx->_scanning = true;
		}
		break;
	case NL80211_CMD_CONNECT: // LINK_UP
		{
			log_(0, "(event) Connect");
			if (!ctx->_up_handler) { break; }

			if (!m.attr_status_code) {
				log_(0, "(event) Unknown connect status");
			} else if (m.attr_status_code.get() == 0) {
				log_(0, "(event) Connection success");
				mih::link_tuple_id lid;
				lid.type = mih::link_type_802_11;
				lid.addr = ctx->_mac;

				if (m.attr_mac) {
					lid.poa_addr = mih::mac_addr(m.attr_mac.get().c_str());
				}

				boost::optional<mih::link_addr> old_router;
				boost::optional<mih::link_addr> new_router;
				boost::optional<bool> ip_renew;
				boost::optional<mih::ip_mob_mgmt> mobility_management;
				ctx->_ios.dispatch(boost::bind(ctx->_up_handler.get(), lid, old_router, new_router, ip_renew, mobility_management));
			} else {
				log_(0, "(event) Connection failure, code ", m.attr_status_code.get());
			}
		}
		break;

//	case NL80211_CMD_DEAUTHENTICATE:
//	case NL80211_CMD_DISASSOCIATE:
	case NL80211_CMD_DISCONNECT: // LINK_DOWN
		{
			log_(0, "(event) Disconnect");
			if (!ctx->_down_handler) { break; }

			unsigned short reason_code = 0; // "local request"
			if (m.attr_reason_code) {
				reason_code = m.attr_reason_code.get();
			}

			mih::link_tuple_id lid;
			lid.type = mih::link_type_802_11;
			lid.addr = ctx->_mac;

			mih::link_dn_reason rs = reason_code_2_dn_reason(reason_code);

			boost::optional<mih::link_addr> old_router;
			ctx->_ios.dispatch(boost::bind(ctx->_down_handler.get(), lid, old_router, rs));
		}
		break;

	case NL80211_CMD_SCAN_ABORTED: // LINK_DETECTED?
		{
			ctx->_scanning = false;
			log_(0, "(event) Scan aborted");
		}
		break;
	case NL80211_CMD_NEW_SCAN_RESULTS: // LINK DETECTED
		{
			ctx->_scanning = false;
			log_(0, "(event) New scan results");
			if (!ctx->_detected_handler) { break; }

			// The multicast message just informs of new results.
			// The complete information must be retrieved with a GET_SCAN.
			scan_results_data d(*ctx);
			fetch_scan_results(d);
			dispatch_strongest_scan_results(d);
		}
		break;

	default:
		break;
	}

	return NL_SKIP;
}

void recv_forever(nlwrap::genl_socket &sock, nlwrap::nl_cb &cb)
{
	while (!cb.finish()) {
		sock.receive(cb);
	}

	throw std::runtime_error("Unexpected netlink error, code " + boost::lexical_cast<std::string>(cb.error_code()));
}

///////////////////////////////////////////////////////////////////////////////

if_80211::if_80211(boost::asio::io_service &ios, mih::mac_addr mac) : _ctx(ios)
{
	// set variables
	_ctx._mac = mac;

	nlwrap::rtnl_link_cache cache;
	nlwrap::rtnl_link link(cache.get_by_addr(mac.address()));

	_ctx._ifindex = link.ifindex();
	_ctx._dev = link.name();

	// initalize socket
	_ctx._family_id = _socket.family_id("nl80211");
	_socket.join_multicast_group("scan");
	_socket.join_multicast_group("mlme");

	_callback.custom(handle_nl_event, &_ctx);

	boost::thread rcv(boost::bind(recv_forever, boost::ref(_socket), boost::ref(_callback)));
}

if_80211::~if_80211()
{
}

unsigned int if_80211::ifindex()
{
	return _ctx._ifindex;
}

std::string if_80211::ifname()
{
	return _ctx._dev;
}

mih::mac_addr if_80211::mac_address()
{
	return _ctx._mac;
}

mih::link_id if_80211::link_id()
{
	mih::link_id id;
	id.type = mih::link_type_802_11;
	id.addr = _ctx._mac;

	return id;
}

bool if_80211::link_up()
{
	nlwrap::rtnl_link_cache cache;
	nlwrap::rtnl_link link(cache.get_by_ifindex(_ctx._ifindex));

	return link.get_flags() & IFF_RUNNING;
}

sint8 if_80211::get_current_rssi(const mih::mac_addr &addr)
{
	log_(0, "(command) Dumping station results");

	nlwrap::genl_socket s;

	nlwrap::genl_msg m(s.family_id("nl80211"), NL80211_CMD_GET_STATION, NLM_F_DUMP);
	m.put_ifindex(_ctx._ifindex);
	m.put_mac(addr.address());

	sint8 rssi = 0;
	nlwrap::nl_cb cb(handle_station_results_signal, static_cast<void *>(&rssi));

	s.send(m);

	while (!cb.finish()) {
		s.receive(cb);
	}

	if (cb.error()) {
		throw std::runtime_error("Error getting station RSSI, code: " + boost::lexical_cast<std::string>(cb.error_code()));
	}

	return rssi;
}

poa_info if_80211::get_poa_info()
{
	scan_results_data d(_ctx);
	fetch_scan_results(d);

	if (!d.associated_index) {
		throw std::runtime_error("Not associated to a POA");
	}

	poa_info i = d.l[d.associated_index.get()];
	mih::mac_addr addr = boost::get<mih::mac_addr>(boost::get<mih::link_addr>(i.id.poa_addr));
	i.signal = get_current_rssi(addr);

	return i;
}

void if_80211::trigger_scan(bool wait)
{
	log_(0, "(command) Triggering scan");

	int command = -1;
	nlwrap::genl_socket s;
	nlwrap::nl_cb cb;

	if (wait) {
		cb.custom(signal_scan_finish_handler, &command);
		s.join_multicast_group("scan");
	} else {
		command = 1;
	}

	nlwrap::genl_msg m(_ctx._family_id, NL80211_CMD_TRIGGER_SCAN, 0);
	m.put_ifindex(_ctx._ifindex);
	m.put_active_scanning();

	s.send(m);

	//while (error > 0 || (wait && command < 0 && !(error < 0))) {
	while ((!cb.finish()) || (command < 0 && !cb.error())) {
		s.receive(cb);
	}

	if (cb.error()) {
		throw std::runtime_error("Netlink error, code: " + boost::lexical_cast<std::string>(cb.error_code()));
	}

	log_(0, "(command) Scan triggered");
}

mih::link_scan_rsp_list if_80211::get_scan_results()
{
	scan_results_data d(_ctx);
	fetch_scan_results(d);

	mih::link_scan_rsp_list l;
	BOOST_FOREACH(poa_info &poa, d.l) {
		mih::link_scan_rsp r;
		r.id = boost::get<mih::link_addr>(poa.id.poa_addr);
		r.net_id = poa.network_id;
		r.signal = poa.signal;
		l.push_back(r);
	}

	return l;
}

mih::op_mode_enum if_80211::get_op_mode()
{
	nlwrap::rtnl_link_cache cache;
	nlwrap::rtnl_link link(cache.get_by_ifindex(_ctx._ifindex));

	if (!(link.get_flags() & IFF_UP)) {
		return mih::op_mode_powered_down;
	}

#ifdef NL80211_CMD_GET_POWER_SAVE
	nlwrap::genl_socket s;
	nlwrap::genl_msg m(_ctx._family_id, NL80211_CMD_GET_POWER_SAVE, 0);
	m.put_ifindex(_ctx._ifindex);

	int operstate = -1;
	nlwrap::nl_cb cb(handle_operstate, &operstate);

	s.send(m);
	while (!cb.finish()) {
		s.receive(cb);
	}

	if (cb.error()) {
		throw std::runtime_error("Error getting op mode, code: " + boost::lexical_cast<std::string>(cb.error_code()));
	}

	if (operstate == NL80211_PS_ENABLED) {
		return odtone::mih::op_mode_power_saving;
	}
#endif /* NL80211_CMD_GET_POWER_SAVE */

	return mih::op_mode_normal;
}

void if_80211::set_op_mode(const mih::link_ac_type_enum &mode)
{
	log_(0, "(command) Setting link op_mode");

	switch (mode) {
	case odtone::mih::link_ac_type_none:
		// nothing
		break;

	case odtone::mih::link_ac_type_power_down:
	case odtone::mih::link_ac_type_power_up:
		{
			nlwrap::rtnl_link change;

			if (mode == odtone::mih::link_ac_type_power_down) {
				change.unset_flags(IFF_UP);
			} else /*if (mode == odtone::mih::link_ac_type_power_up) */{
				change.set_flags(IFF_UP);
			}

			nlwrap::rtnl_link_cache cache;
			nlwrap::rtnl_link link(cache.get_by_ifindex(_ctx._ifindex));
			cache.change(link, change);
		}
		break;
	case odtone::mih::link_ac_type_disconnect:
		{
			nlwrap::genl_socket s;
			nlwrap::genl_msg m(_ctx._family_id, NL80211_CMD_DISCONNECT, 0);
			m.put_ifindex(_ctx._ifindex);

			nlwrap::nl_cb cb;

			s.send(m);
			while (!cb.finish()) {
				s.receive(cb);
			}

			if (cb.error()) {
				throw std::runtime_error("Error disconnecting, code: " + boost::lexical_cast<std::string>(cb.error_code()));
			}
		}
		break;
	case odtone::mih::link_ac_type_low_power:
#ifdef NL80211_CMD_SET_POWER_SAVE
		{
			nlwrap::genl_socket s;
			nlwrap::genl_msg m(_ctx._family_id, NL80211_CMD_SET_POWER_SAVE, 0);
			m.put_ifindex(_ctx._ifindex);
			m.put_ps_state(NL80211_PS_ENABLED);

			nlwrap::nl_cb cb;

			s.send(m);
			while (!cb.finish()) {
				s.receive(cb);
			}

			if (cb.error()) {
				throw std::runtime_error("Error setting power_save, code: " + boost::lexical_cast<std::string>(cb.error_code()));
			}
		}
		break;
#endif /* NL80211_CMD_SET_POWER_SAVE */
	default:
		throw std::runtime_error("Mode not supported");
		break;
	}
}

odtone::uint if_80211::get_packet_error_rate()
{
	nlwrap::rtnl_link_cache cache;
	nlwrap::rtnl_link link(cache.get_by_ifindex(_ctx._ifindex));

	return link.tx_errors() / link.tx_packets();
}

odtone::uint if_80211::get_current_data_rate(mih::mac_addr &addr)
{
	log_(0, "(command) Getting current data rate");

	nlwrap::genl_socket s;

	nlwrap::genl_msg m(s.family_id("nl80211"), NL80211_CMD_GET_STATION, NLM_F_DUMP);
	m.put_ifindex(_ctx._ifindex);
	m.put_mac(addr.address());

	odtone::uint rate = 0;
	nlwrap::nl_cb cb(handle_station_results_rate, static_cast<void *>(&rate));

	s.send(m);

	while (!cb.finish()) {
		s.receive(cb);
	}

	if (cb.error()) {
		throw std::runtime_error("Error getting data rate, code: " + boost::lexical_cast<std::string>(cb.error_code()));
	}

	return rate * 100;
}

boost::optional<poa_info> if_80211::known_bssid(mih::mac_addr &addr)
{
	boost::shared_lock<boost::shared_mutex> lock(_ctx._seen_bssids_access);

	auto found = _ctx._seen_bssids.find(addr.address());
	if (found != _ctx._seen_bssids.end()) {
		return found->second;
	} else {
		return boost::optional<poa_info>();
	}
}

void if_80211::clear_addresses()
{
	nlwrap::rtnl_addr_cache addr_cache;
	nlwrap::rtnl_addr addr;

	addr.set_ifindex(_ctx._ifindex);
	addr_cache.clear(addr);
}

void if_80211::add_addresses(const std::vector<mih::ip_info> &addrs)
{
	std::vector<mih::ip_info> default_gateways;
	nlwrap::rtnl_addr_cache addr_cache;

	for (auto it = addrs.begin(); it != addrs.end(); it++) {
		// address type/family
		nlwrap::nl_addr::ADDRESS_FAMILY addr_family;
		mih::ip_addr::type_ip_enum ip_type;
		if (it->subnet.ipaddr.type() == mih::ip_addr::ipv4) {
			addr_family = nlwrap::nl_addr::ADDRESS_FAMILY_AF_INET;
			ip_type = mih::ip_addr::ipv4;
		} else if (it->subnet.ipaddr.type() == mih::ip_addr::ipv6) {
			addr_family = nlwrap::nl_addr::ADDRESS_FAMILY_AF_INET6;
			ip_type = mih::ip_addr::ipv6;
		} else {
			log_(0, "(command) Unknown address type/family");
			throw std::runtime_error("Unknown address type/family");
		}

		// queue the default gateway
		mih::ip_info gw;
		gw.gateway = it->gateway;
		std::vector<unsigned char> no_target_raw(it->subnet.ipaddr.length(), 0);
		gw.subnet.ipaddr = mih::ip_addr(ip_type, &no_target_raw[0], no_target_raw.size());
		gw.subnet.ipprefixlen = 0;
		default_gateways.push_back(gw);

		// add the ip to the interface
		const unsigned char *target_raw = static_cast<const unsigned char *>(it->subnet.ipaddr.get());
		std::vector<unsigned char> target(target_raw, target_raw + it->subnet.ipaddr.length());
		nlwrap::nl_addr local_addr(addr_family, &target[0], target.size());
		nlwrap::rtnl_addr addr;
		addr.set_local(local_addr);
		addr.set_ifindex(_ctx._ifindex);
		addr.set_prefixlen(it->subnet.ipprefixlen);

		addr_cache.add(addr);
	}

	add_routes(default_gateways);
}

void if_80211::clear_routes()
{
	nlwrap::rtnl_route_cache route_cache;
	nlwrap::rtnl_route route;

	route.set_oifindex(_ctx._ifindex);
	route_cache.clear(route);
}

void if_80211::add_routes(const std::vector<mih::ip_info> &routes)
{
	nlwrap::rtnl_route_cache route_cache;

	for (auto it = routes.begin(); it != routes.end(); it++) {
		// address type/family
		nlwrap::nl_addr::ADDRESS_FAMILY addr_family;
		nlwrap::rtnl_route::ROUTE_FAMILY route_family;
		if (it->subnet.ipaddr.type() == mih::ip_addr::ipv4) {
			addr_family = nlwrap::nl_addr::ADDRESS_FAMILY_AF_INET;
			route_family = nlwrap::rtnl_route::ROUTE_FAMILY_AF_INET;
		} else if (it->subnet.ipaddr.type() == mih::ip_addr::ipv6) {
			addr_family = nlwrap::nl_addr::ADDRESS_FAMILY_AF_INET6;
			route_family = nlwrap::rtnl_route::ROUTE_FAMILY_AF_INET6;
		} else {
			log_(0, "(command) Unknown address type/family");
			throw std::runtime_error("Unknown address type/family");
		}

		// create the destination network
		const unsigned char *_target_raw = static_cast<const unsigned char *>(it->subnet.ipaddr.get());
		std::vector<unsigned char> target_raw(_target_raw, _target_raw + it->subnet.ipaddr.length());
		nlwrap::nl_addr target(addr_family, &target_raw[0], target_raw.size(), it->subnet.ipprefixlen);

		// gateway
		const unsigned char *_gateway_raw = static_cast<const unsigned char*>(it->gateway.get());
		std::vector<unsigned char> gateway_raw(_gateway_raw, _gateway_raw + it->gateway.length());
		nlwrap::nl_addr gateway(addr_family, &gateway_raw[0], gateway_raw.size());

		// route
		nlwrap::rtnl_route::ROUTE_SCOPE route_scope(nlwrap::rtnl_route::ROUTE_SCOPE_UNIVERSE);
		nlwrap::rtnl_route route(_ctx._ifindex, target, gateway, route_scope, route_family);

		route_cache.add(route);
	}
}

void if_80211::link_up_callback(link_up_handler h)
{
	_ctx._up_handler = h;
}

void if_80211::link_down_callback(link_down_handler h)
{
	_ctx._down_handler = h;
}

void if_80211::link_detected_callback(link_detected_handler h)
{
	_ctx._detected_handler = h;
}

// EOF ////////////////////////////////////////////////////////////////////////
