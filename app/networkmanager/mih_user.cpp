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

#include <odtone/mih/request.hpp>
#include <odtone/mih/response.hpp>
#include <odtone/mih/indication.hpp>
#include <odtone/mih/confirm.hpp>
#include <odtone/mih/tlv_types.hpp>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <iostream>

using namespace odtone::networkmanager;

mih_user::mih_user(const odtone::mih::config &cfg, boost::asio::io_service &io, NetworkManager &nm) :
	_mihf(cfg, io, boost::bind(&mih_user::event_handler, this, _1, _2)),
	_nm(nm),
	log_("mih_usr", std::cout)
{
	odtone::mih::message m;

	boost::optional<odtone::mih::mih_cmd_list> supp_cmd;

	m << odtone::mih::indication(odtone::mih::indication::user_register)
	    & odtone::mih::tlv_command_list(supp_cmd);
	m.destination(odtone::mih::id("local-mihf"));

	_mihf.async_send(m, boost::bind(&mih_user::user_reg_handler, this, boost::cref(cfg), _2));
}

mih_user::~mih_user()
{
}

void mih_user::user_reg_handler(const odtone::mih::config &cfg, const boost::system::error_code &ec)
{
	log_(0, "MIH-User registered, status: ", ec.message());

	if (ec) {
		throw "Error registering to local MIHF";
	}

	// get the local links
	odtone::mih::message m;
	m << odtone::mih::request(odtone::mih::request::capability_discover);
	m.destination(odtone::mih::id("local-mihf"));

	_mihf.async_send(m, boost::bind(&mih_user::capability_discover_confirm, this, _1, _2));
}

/**
 * Capability Discover handler.
 *
 * @param msg Received message.
 * @param ec Error Code.
 */
void mih_user::capability_discover_confirm(odtone::mih::message& msg, const boost::system::error_code& ec)
{
	log_(0, "Received local MIHF capabilities, status: ", ec.message());

	if (ec) {
		return;
	}

	odtone::mih::status st;
	boost::optional<odtone::mih::net_type_addr_list> ntal;
	boost::optional<odtone::mih::mih_evt_list> evt;

	msg >> odtone::mih::confirm()
		& odtone::mih::tlv_status(st)
		& odtone::mih::tlv_net_type_addr_list(ntal)
		& odtone::mih::tlv_event_list(evt);

	// TODO fix to other technologies
	if (ntal) {
		BOOST_FOREACH(mih::net_type_addr &l, ntal.get()) {
			mih::link_type *lt = boost::get<mih::link_type>(&l.nettype.link);
			if (!lt) {
				log_(0, "Link does not have a type");
				break;
			}

			if (*lt == mih::link_type_802_11) {
				mih::mac_addr *mac = boost::get<mih::mac_addr>(&l.addr);
				if (mac) {
					log_(0, "Adding 802.11 link with address ", mac->address());
					if_80211 fi(*mac);
					_nm.add_wifi_device(fi);
				} else {
					log_(0, "Found 802.11, but no mac address");
				}
			} else {
				log_(0, "Unsupported device type");
			}
		}
	}
}

void mih_user::event_handler(odtone::mih::message &msg, const boost::system::error_code &ec)
{
	log_(0, "Event received, status: ", ec.message());

	if (ec) {
		return;
	}

	switch (msg.mid()) {
	case odtone::mih::indication::link_up:
		log_(0, "Received a link_up event");
		break;

	case odtone::mih::indication::link_down:
		log_(0, "Received a link_down event");
		break;

	case odtone::mih::indication::link_detected:
		log_(0, "Received a link_detected event");
		break;

	case odtone::mih::indication::link_going_down:
		log_(0, "Received a link_going_down event");
		break;

	case odtone::mih::indication::link_handover_imminent:
		log_(0, "Received a link_handover_imminent event");
		break;
	case odtone::mih::indication::link_handover_complete:
		log_(0, "Received a link_handover_complete event");
		break;
	default:
		log_(0, "Received unknown/unsupported event");
	}
}
