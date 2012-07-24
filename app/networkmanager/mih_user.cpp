//==============================================================================
// Brief   : NetworkManager MIH User endpoint
// Authors : André Prata <andreprata@av.it.pt>
//------------------------------------------------------------------------------
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

#include "mih_user.hpp"

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <iostream>

using namespace odtone::networkmanager;

mih_user::mih_user(const mih::config &cfg, boost::asio::io_service &io,
                   const default_handler &h, const new_device_handler &d) :
	_mihf(cfg, io, h),
	log_("mih_usr", std::cout),
	_device_handler(d),
	_h(h)
{
	mih::message m;

	boost::optional<mih::mih_cmd_list> supp_cmd;

	m << mih::indication(mih::indication::user_register)
	    & mih::tlv_command_list(supp_cmd);
	m.destination(mih::id("local-mihf"));

	_mihf.async_send(m, boost::bind(&mih_user::user_reg_handler, this, _2));
}

mih_user::~mih_user()
{
}

void mih_user::power_down(const default_handler &h, const mih::link_tuple_id &lti)
{
	mih::link_action_req 	lar;
	mih::link_action_list	larl;

	lar.id = lti;
	lar.action.type = mih::link_ac_type_power_down;
	lar.ex_time = 0;

	larl.push_back(lar);

	mih::message m;
	m << mih::request(mih::request::link_actions)
		& mih::tlv_link_action_list(larl);
	m.destination(mih::id("local-mihf"));

	_mihf.async_send(m, h);
}

void mih_user::power_up(const default_handler &h, const mih::link_tuple_id &lti, bool scan)
{
	mih::link_action_req 	lar;
	mih::link_action_list	larl;

	lar.id = lti;
	lar.action.type = mih::link_ac_type_power_up;
	lar.ex_time = 0;

	if (scan) {
		lar.action.attr.set(mih::link_ac_attr_scan);
	}

	larl.push_back(lar);

	mih::message m;
	m << mih::request(mih::request::link_actions)
		& mih::tlv_link_action_list(larl);
	m.destination(mih::id("local-mihf"));

	_mihf.async_send(m, h);
}

void mih_user::disconnect(const default_handler &h, const mih::link_tuple_id &lti)
{
	mih::link_action_req 	lar;
	mih::link_action_list	larl;

	lar.id = lti;
	lar.action.type = mih::link_ac_type_disconnect;
	lar.ex_time = 0;

	larl.push_back(lar);

	mih::message m;
	m << mih::request(mih::request::link_actions)
		& mih::tlv_link_action_list(larl);
	m.destination(mih::id("local-mihf"));

	_mihf.async_send(m, h);
}

void mih_user::scan(const default_handler &h, const mih::link_tuple_id &lti)
{
	mih::link_action_req 	lar;
	mih::link_action_list	larl;

	lar.id = lti;
	lar.addr = lti.addr;
	lar.action.type = mih::link_ac_type_none;
	lar.action.attr.set(mih::link_ac_attr_scan);
	lar.ex_time = 0;

	larl.push_back(lar);

	mih::message m;
	m << mih::request(mih::request::link_actions)
		& mih::tlv_link_action_list(larl);
	m.destination(mih::id("local-mihf"));

	_mihf.async_send(m, h);
}

void mih_user::link_conf(const default_handler &h,
                         const mih::link_tuple_id &lti,
                         const boost::optional<mih::link_addr> &poa,
                         const mih::configuration_list &conf)
{
	mih::message m;

	m << mih::request(mih::request::link_conf)
		& mih::tlv_link_identifier(lti)
		& mih::tlv_poa(poa)
		& mih::tlv_configuration_list(conf);
	m.destination(mih::id("local-mihf"));

	_mihf.async_send(m, h);
}

void mih_user::l3_conf(const default_handler &h,
                       const mih::link_tuple_id &lti,
                       const mih::ip_cfg_methods &cfg_methods,
                       const boost::optional<mih::ip_info_list> &address_list,
                       const boost::optional<mih::ip_info_list> &route_list,
                       const boost::optional<mih::ip_addr_list> &dns_list,
                       const boost::optional<mih::fqdn_list> &domain_list)
{
	mih::message m;

	m << mih::request(mih::request::l3_conf)
		& mih::tlv_link_identifier(lti)
		& mih::tlv_ip_cfg_methods(cfg_methods)
		& mih::tlv_ip_addr_list(address_list)
		& mih::tlv_ip_route_list(route_list)
		& mih::tlv_ip_dns_list(dns_list)
		& mih::tlv_fqdn_list(domain_list);
	m.destination(mih::id("local-mihf"));

	_mihf.async_send(m, h);
}

void mih_user::user_reg_handler(const boost::system::error_code &ec)
{
	log_(0, "MIH-User registered, status: ", ec.message());

	if (ec) {
		throw "Error registering to local MIHF";
	}

	// get the local links
	mih::message m;
	m << mih::request(mih::request::capability_discover);
	m.destination(mih::id("local-mihf"));

	_mihf.async_send(m, boost::bind(&mih_user::capability_discover_confirm, this, _1, _2));
}

/**
 * Capability Discover handler.
 *
 * @param msg Received message.
 * @param ec Error Code.
 */
void mih_user::capability_discover_confirm(mih::message& msg, const boost::system::error_code& ec)
{
	log_(0, "Received local MIHF capabilities, status: ", ec.message());

	if (ec) {
		return;
	}

	mih::status st;
	boost::optional<mih::net_type_addr_list> ntal;
	boost::optional<mih::mih_evt_list> evt;

	msg >> mih::confirm()
		& mih::tlv_status(st)
		& mih::tlv_net_type_addr_list(ntal)
		& mih::tlv_event_list(evt);

	// TODO fix to other technologies
	if (!ntal) {
		return;
	}

	BOOST_FOREACH (mih::net_type_addr &l, ntal.get()) {
		mih::link_tuple_id li;
		li.type = boost::get<mih::link_type>(l.nettype.link);
		li.addr = l.addr;

		_device_handler(l.nettype, l.addr);

		// attempt to subscribe events of interest
		mih::mih_evt_list wanted_evts;
		wanted_evts.set(mih::mih_evt_link_detected);
		wanted_evts.set(mih::mih_evt_link_down);
		wanted_evts.set(mih::mih_evt_link_going_down);
		wanted_evts.set(mih::mih_evt_link_parameters_report);
		wanted_evts.set(mih::mih_evt_link_up);
		wanted_evts.set(mih::mih_evt_link_conf_required);

		mih::message m;
		m << mih::request(mih::request::event_subscribe)
			& mih::tlv_link_identifier(li)
			& mih::tlv_event_list(wanted_evts);
		m.destination(mih::id("local-mihf"));

		_mihf.async_send(m, boost::bind(&mih_user::event_subscribe_response, this, _1, _2));

		// also configure periodic reports
		mih::message p;

		mih::link_cfg_param       lcp;
		mih::link_cfg_param_list  lcpl;

		mih::link_param_802_11 lp = mih::link_param_802_11_rssi;
		lcp.type = lp;
		lcp.timer_interval = 30000; // every 30 sec
		lcp.action = mih::th_action_normal;
		lcpl.push_back(lcp);

		p << mih::request(mih::request::link_configure_thresholds)
			& mih::tlv_link_identifier(li)
			& mih::tlv_link_cfg_param_list(lcpl);
		p.destination(mih::id("local-mihf"));

		_mihf.async_send(p, boost::bind(&mih_user::configure_thresholds_response, this, _1, _2));
	}
}

void mih_user::event_subscribe_response(mih::message &msg, const boost::system::error_code &ec)
{
	log_(0, "Received event subscription response, status: ", ec.message());

	// do nothing
}

void mih_user::configure_thresholds_response(mih::message &msg, const boost::system::error_code &ec)
{
	log_(0, "Received configure thresholds response, status: ", ec.message());

	// do nothing
}
