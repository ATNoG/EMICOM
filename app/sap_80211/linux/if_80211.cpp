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

#include "nlwrap/nlwrap.hpp"
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

	boost::optional<uint> associated_index;
	if_80211::ctx_data &_ctx;

	scan_results_data(if_80211::ctx_data &ctx) : _ctx(ctx) {}
};

int handle_operstate(nl_msg *msg, void *arg)
{
	nlwrap::genl_msg m(msg);
	int *state = static_cast<int *>(arg);

	if (m.attr_ps_state) {
		*state = m.attr_ps_state.get();
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

		i.network_id = m.ie_ssid.get();
		i.channel_id = frequency_to_channel_id(m.bss_frequency.get());
		i.id.type = mih::link_type_802_11;
		i.id.addr = d->_ctx._mac;
		i.id.poa_addr = mih::mac_addr(m.bss_bssid.get().c_str());
		i.signal = m.bss_signal_mbm.get();
		i.data_rate = m.ie_max_data_rate.get();
		i.mih_capabilities = mih::link_mihcap_flag();
		i.net_capabilities = caps;
		//i.sinr = 0;
		// SINR cannot be obtained, since we can only get the signal strength
		// and the noise value (through NL80211_CMD_GET_SURVEY). (missing "interference")

		d->l.push_back(i);
	} catch(...) {
		log_(0, "(command) Error parsing scan dump message");
		return NL_SKIP;
	}

	return NL_SKIP;
}

void fetch_scan_results(scan_results_data &data)
{
	log_(0, "(command) Dumping scan results");

	nlwrap::genl_socket s;

	nlwrap::genl_msg m(s.family_id("nl80211"), NL80211_CMD_GET_SCAN, NLM_F_DUMP);
	m.put_ifindex(data._ctx._ifindex);

	nlwrap::genl_cb cb(handle_scan_results, static_cast<void *>(&data));

	s.send(m);

	while (!cb.finish()) {
		s.receive(cb);
	}

	if (cb.error()) {
		throw "Error, code: " + boost::lexical_cast<std::string>(cb.error_code());
	}

	log_(0, "(command) Dumped ", data.l.size(), " scan results");
}

void dispatch_strongest_scan_results(scan_results_data &d)
{
	std::sort(d.l.begin(), d.l.end(), // sort by strongest signal
		[](const poa_info &a, const poa_info &b)
		{
			const sint8 *a_dbm = boost::get<sint8>(&a.signal);
			const sint8 *b_dbm = boost::get<sint8>(&b.signal);
			if (a_dbm && b_dbm) {
				return *a_dbm > *b_dbm;
			} else {
				const mih::percentage *a_pct = boost::get<mih::percentage>(&a.signal);
				const mih::percentage *b_pct = boost::get<mih::percentage>(&b.signal);
				if (a_pct && b_pct) {
					uint _a = *((const uint *)a_pct);
					uint _b = *((const uint *)b_pct);
					return _a > _b;
				}
			}
			throw "SIG_STRENGTH info not available for sorting";
		});

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

void recv_forever(nlwrap::genl_socket &sock, nlwrap::genl_cb &cb)
{
	while (!cb.finish()) {
		sock.receive(cb);
	}

	throw "Unexpected netlink error, code " + boost::lexical_cast<std::string>(cb.error_code());
}

///////////////////////////////////////////////////////////////////////////////

if_80211::if_80211(boost::asio::io_service &ios, mih::mac_addr mac) : _ctx(ios)
{
	// set variables
	_ctx._mac = mac;

	nlwrap::rtnl_link_cache cache;
	nlwrap::rtnl_link link(cache.get_by_addr(mac.address()));

	_ctx._ifindex = link.ifindex();

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

poa_info if_80211::get_poa_info()
{
	scan_results_data d(_ctx);
	fetch_scan_results(d);

	if (!d.associated_index) {
		throw "Not associated to a POA";
	}

	return d.l[d.associated_index.get()];
}

void if_80211::trigger_scan(bool wait)
{
	log_(0, "(command) Triggering scan");

	int command = -1;
	nlwrap::genl_socket s;
	nlwrap::genl_cb cb;

	if (wait) {
		cb.custom(signal_scan_finish_handler, &command);
		s.join_multicast_group("scan");
	} else {
		command = 1;
	}

	nlwrap::genl_msg m(_ctx._family_id, NL80211_CMD_TRIGGER_SCAN, 0);
	m.put_ifindex(_ctx._ifindex);

	s.send(m);

	//while (error > 0 || (wait && command < 0 && !(error < 0))) {
	while ((!cb.finish()) || (command < 0 && !cb.error())) {
		s.receive(cb);
	}

	if (cb.error()) {
		throw "Netlink error, code: " + boost::lexical_cast<std::string>(cb.error_code());
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

	nlwrap::genl_socket s;
	nlwrap::genl_msg m(_ctx._family_id, NL80211_CMD_GET_POWER_SAVE, 0);
	m.put_ifindex(_ctx._ifindex);

	int operstate = -1;
	nlwrap::genl_cb cb(handle_operstate, &operstate);

	s.send(m);
	while (!cb.finish()) {
		s.receive(cb);
	}

	if (cb.error()) {
		throw "Error getting op mode, code: " + boost::lexical_cast<std::string>(cb.error_code());
	}

	if (operstate == NL80211_PS_ENABLED) {
		return odtone::mih::op_mode_power_saving;
	}

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

			nlwrap::genl_cb cb;

			s.send(m);
			while (!cb.finish()) {
				s.receive(cb);
			}

			if (cb.error()) {
				throw "Error disconnecting, code: " + boost::lexical_cast<std::string>(cb.error_code());
			}
		}
		break;
	case odtone::mih::link_ac_type_low_power:
		{
			nlwrap::genl_socket s;
			nlwrap::genl_msg m(_ctx._family_id, NL80211_CMD_SET_POWER_SAVE, 0);
			m.put_ifindex(_ctx._ifindex);
			m.put_ps_state(NL80211_PS_ENABLED);

			nlwrap::genl_cb cb;

			s.send(m);
			while (!cb.finish()) {
				s.receive(cb);
			}

			if (cb.error()) {
				throw "Error setting power_save, code: " + boost::lexical_cast<std::string>(cb.error_code());
			}
		}
		break;
	default:
		throw "Mode not supported";
		break;
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