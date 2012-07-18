//=============================================================================
// Brief   : Netlink interface for 802.3 devices
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

#include "if_8023.hpp"
#include <boost/thread.hpp>
#include <boost/scope_exit.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <odtone/mih/tlv_types.hpp>

#include <odtone/logger.hpp>
#include <iostream>
#include <stdexcept>

using namespace odtone;

static logger log_("if_8023", std::cout);

///////////////////////////////////////////////////////////////////////////////

#define IF_OPER_UP 6
int handle_nl_event(nl_msg *msg, void *arg)
{
	if_8023::ctx_data *ctx = static_cast<if_8023::ctx_data *>(arg);

	nlwrap::nl_msg m(msg);
	::ifinfomsg *ifi = static_cast< ::ifinfomsg * >(m.payload());

	if (m.type() != RTM_NEWLINK) {
		return NL_SKIP;
	}

	if (ifi->ifi_index != ctx->_ifindex) {
		return NL_SKIP;
	}

	mih::link_tuple_id lid;
	lid.type = mih::link_type_ethernet;
	lid.addr = ctx->_mac;

	boost::optional<mih::link_addr> old_router;

	nlwrap::rtnl_link_cache cache;
	nlwrap::rtnl_link link(cache.get_by_ifindex(ctx->_ifindex));

	bool active = link.get_operstate() == IF_OPER_UP;
	if (active != ctx->_active) {
		ctx->_active = active;

		if (active) {
			log_(0, "(event) IF_OPER_UP");

			boost::optional<mih::link_addr> new_router;
			boost::optional<bool> ip_renew;
			boost::optional<mih::ip_mob_mgmt> mobility_management;
			ctx->_ios.dispatch(boost::bind(ctx->_up_handler.get(),
			                               lid,
			                               old_router,
			                               new_router,
			                               ip_renew,
			                               mobility_management));
		} else {
			log_(0, "(event) ~IF_OPER_UP");
			mih::link_dn_reason rs = mih::link_dn_reason(mih::link_dn_reason_explicit_disconnect);;

			ctx->_ios.dispatch(boost::bind(ctx->_down_handler.get(), lid, old_router, rs));
		}
	}

	return NL_SKIP;
}

void recv_forever(nlwrap::rtnl_socket &sock, nlwrap::nl_cb &cb)
{
	while (!cb.finish()) {
		sock.receive(cb);
	}

	throw std::runtime_error("Unexpected netlink error, code " + boost::lexical_cast<std::string>(cb.error_code()));
}

///////////////////////////////////////////////////////////////////////////////

if_8023::if_8023(boost::asio::io_service &ios, mih::mac_addr mac) : _ctx(ios)
{
	// set variables
	_ctx._mac = mac;

	nlwrap::rtnl_link_cache cache;
	nlwrap::rtnl_link link(cache.get_by_addr(mac.address()));

	_ctx._ifindex = link.ifindex();
	_ctx._dev = link.name();

	_ctx._active = link.get_operstate() == IF_OPER_UP;

	// initalize socket
	_socket.join_multicast_group(RTMGRP_LINK);
	_callback.custom(handle_nl_event, &_ctx);

	boost::thread rcv(boost::bind(recv_forever, boost::ref(_socket), boost::ref(_callback)));
}

if_8023::~if_8023()
{
}

unsigned int if_8023::ifindex()
{
	return _ctx._ifindex;
}

std::string if_8023::ifname()
{
	return _ctx._dev;
}

mih::mac_addr if_8023::mac_address()
{
	return _ctx._mac;
}

mih::link_id if_8023::link_id()
{
	mih::link_id id;
	id.type = mih::link_type_ethernet;
	id.addr = _ctx._mac;

	return id;
}

bool if_8023::link_up()
{
	nlwrap::rtnl_link_cache cache;
	nlwrap::rtnl_link link(cache.get_by_ifindex(_ctx._ifindex));

	return link.get_flags() & IFF_RUNNING;
}

mih::op_mode_enum if_8023::get_op_mode()
{
	nlwrap::rtnl_link_cache cache;
	nlwrap::rtnl_link link(cache.get_by_ifindex(_ctx._ifindex));

	if (!(link.get_flags() & IFF_UP)) {
		return mih::op_mode_powered_down;
	}

	// No powersave support

	return mih::op_mode_normal;
}

void if_8023::set_op_mode(const mih::link_ac_type_enum &mode)
{
	log_(0, "(command) Setting link op_mode");

	nlwrap::rtnl_link change;

	switch (mode) {
	case odtone::mih::link_ac_type_none:
		return;
		break;

	case odtone::mih::link_ac_type_disconnect:
		//change.unset_flags(IFF_UP);
		//break;

	case odtone::mih::link_ac_type_power_down:
		change.unset_flags(IFF_UP);
		break;

	case odtone::mih::link_ac_type_power_up:
		change.set_flags(IFF_UP);
		break;

	case odtone::mih::link_ac_type_low_power:
	default:
		throw std::runtime_error("Mode not supported");
		break;
	}

	nlwrap::rtnl_link_cache cache;
	nlwrap::rtnl_link link(cache.get_by_ifindex(_ctx._ifindex));
	cache.change(link, change);
}

void if_8023::clear_addresses()
{
	nlwrap::rtnl_addr_cache addr_cache;
	nlwrap::rtnl_addr addr;

	addr.set_ifindex(_ctx._ifindex);
	addr_cache.clear(addr);
}

void if_8023::add_addresses(const std::vector<mih::ip_info> &addrs)
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

void if_8023::clear_routes()
{
	nlwrap::rtnl_route_cache route_cache;
	nlwrap::rtnl_route route;

	route.set_oifindex(_ctx._ifindex);
	route_cache.clear(route);
}

void if_8023::add_routes(const std::vector<mih::ip_info> &routes)
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

void if_8023::link_up_callback(const link_up_handler &h)
{
	_ctx._up_handler = h;
}

void if_8023::link_down_callback(const link_down_handler &h)
{
	_ctx._down_handler = h;
}

// EOF ////////////////////////////////////////////////////////////////////////
