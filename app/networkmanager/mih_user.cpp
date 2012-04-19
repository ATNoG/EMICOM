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

mih_user::mih_user(const mih::config &cfg, boost::asio::io_service &io, NetworkManager &nm) :
	_mihf(cfg, io, boost::bind(&mih_user::event_handler, this, _1, _2)),
	_nm(nm),
	log_("mih_usr", std::cout)
{
	mih::message m;

	boost::optional<mih::mih_cmd_list> supp_cmd;

	m << mih::indication(mih::indication::user_register)
	    & mih::tlv_command_list(supp_cmd);
	m.destination(mih::id("local-mihf"));

	_mihf.async_send(m, boost::bind(&mih_user::user_reg_handler, this, boost::cref(cfg), _2));
}

mih_user::~mih_user()
{
}

void mih_user::user_reg_handler(const mih::config &cfg, const boost::system::error_code &ec)
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
				_nm.add_802_11_device(*mac);

				li.type = mih::link_type_802_11;
				li.addr = *mac;
			} else {
				log_(0, "Found 802.11, but no mac address");
			}
		} else {
			log_(0, "Unsupported device type");
		}

		mih::mih_evt_list wanted_evts;
		wanted_evts.set(mih::mih_evt_link_detected);
		wanted_evts.set(mih::mih_evt_link_down);
		wanted_evts.set(mih::mih_evt_link_going_down);
		wanted_evts.set(mih::mih_evt_link_parameters_report);
		wanted_evts.set(mih::mih_evt_link_up);

		// attempt to subscribe events of interest
		mih::message m;
		m << mih::request(mih::request::event_subscribe)
			& mih::tlv_link_identifier(li)
			& mih::tlv_event_list(wanted_evts);
		m.destination(mih::id("local-mihf"));

		_mihf.async_send(m, boost::bind(&mih_user::event_subscribe_response, this, _1, _2));
	}
}

void mih_user::event_subscribe_response(mih::message &msg, const boost::system::error_code &ec)
{
	log_(0, "Received event subscription response, status: ", ec.message());

	// do nothing
}

void mih_user::event_handler(mih::message &msg, const boost::system::error_code &ec)
{
	log_(0, "Event received, status: ", ec.message());

	if (ec) {
		return;
	}

	switch (msg.mid()) {
	case mih::indication::link_up:
	{
		log_(0, "Received a link_up event");

		mih::link_tuple_id up_link;
		boost::optional<mih::link_addr> __old_rout, __new_rout;
		boost::optional<bool> __ip_renew;
		boost::optional<mih::ip_mob_mgmt> __ip_mob;

		msg >> mih::indication()
			& mih::tlv_link_identifier(up_link)
			& mih::tlv_old_access_router(__old_rout)
			& mih::tlv_new_access_router(__new_rout)
			& mih::tlv_ip_renewal_flag(__ip_renew)
			& mih::tlv_ip_mob_mgmt(__ip_mob);

		mih::mac_addr *dev = boost::get<mih::mac_addr>(&up_link.addr);
		if (!dev) {
			log_(0, "Unable to determine device address.");
			break;
		}

		mih::mac_addr *poa;
		mih::link_addr *_poa = boost::get<mih::link_addr>(&up_link.poa_addr);
		if (_poa) {
			poa = boost::get<mih::mac_addr>(_poa);
			if (!poa) {
				log_(0, "Unable to determine poa address");
				break;
			}
		} else {
			log_(0, "Unable to determine poa address");
			break;
		}

		_nm.link_up(*dev, *poa);
	}
	break;

	case mih::indication::link_down:
	{
		log_(0, "Received a link_down event");

		mih::link_tuple_id down_link;
		boost::optional<mih::link_addr> __old_rout;
		mih::link_dn_reason __dn_reason;

		msg >> mih::indication()
			& mih::tlv_link_identifier(down_link)
			& mih::tlv_old_access_router(__old_rout)
			& mih::tlv_link_dn_reason(__dn_reason);

		mih::mac_addr *dev = boost::get<mih::mac_addr>(&down_link.addr);
		if (!dev) {
			log_(0, "Unable to determine device address.");
			break;
		}

		_nm.link_down(*dev);
	}
	break;

	case mih::indication::link_detected:
		log_(0, "Received a link_detected event");
		_nm.new_accesspoints_detected();
		break;

	case mih::indication::link_going_down:
		// TODO: notify networkmanager
		log_(0, "Received a link_going_down event");
		break;

	case mih::indication::link_handover_imminent:
		log_(0, "Received a link_handover_imminent event");
		break;
	case mih::indication::link_handover_complete:
		log_(0, "Received a link_handover_complete event");
		break;
	default:
		log_(0, "Received unknown/unsupported event");
	}
}
