//==============================================================================
// Brief   : NetworkManager Device interface implementation
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

#include "Device.hpp"
#include <iostream>

using namespace odtone::networkmanager;

Device::Device(DBus::Connection &connection, const char* path, mih_user &ctrl, mih::link_tuple_id &lti)
	: DBus::ObjectAdaptor(connection, path), _ctrl(ctrl), _lti(lti),  _dbus_path(path), log_(_dbus_path.c_str(), std::cout)
{
}

Device::~Device()
{
	std::cerr << "Going to life!!" << std::endl;
}

void Device::Disconnect()
{
	log_(0, "Disconnecting");

	// assume success
	state(NM_DEVICE_STATE_DISCONNECTED, NM_DEVICE_STATE_REASON_UNKNOWN);

	_ctrl.disconnect(
		[&](mih::message &pm, const boost::system::error_code &ec) {
			mih::status st;
			pm >> mih::confirm(mih::confirm::link_actions)
				& mih::tlv_status(st);

			if (st != mih::status_success) {
				state(NM_DEVICE_STATE_ACTIVATED, NM_DEVICE_STATE_REASON_UNKNOWN);
			}
		}, _lti);
}

void Device::Enable()
{
	log_(0, "Enabling");

	// assume success
	state(NM_DEVICE_STATE_ACTIVATED, NM_DEVICE_STATE_REASON_UNKNOWN);

	_ctrl.power_up(
		[&](mih::message &pm, const boost::system::error_code &ec) {
			mih::status st;
			pm >> mih::confirm(mih::confirm::link_actions)
				& mih::tlv_status(st);

			if (st != mih::status_success) {
				state(NM_DEVICE_STATE_DISCONNECTED, NM_DEVICE_STATE_REASON_UNKNOWN);
			}
		}, _lti);
}

void Device::Disable()
{
	log_(0, "Disabling");

	// assume success
	state(NM_DEVICE_STATE_DISCONNECTED, NM_DEVICE_STATE_REASON_UNKNOWN);

	_ctrl.power_down(
		[&](mih::message &pm, const boost::system::error_code &ec) {
			mih::status st;
			pm >> mih::confirm(mih::confirm::link_actions)
				& mih::tlv_status(st);

			if (st != mih::status_success) {
				state(NM_DEVICE_STATE_ACTIVATED, NM_DEVICE_STATE_REASON_UNKNOWN);
			}
		}, _lti);
}

void Device::state(NM_DEVICE_STATE newstate, NM_DEVICE_STATE_REASON reason)
{
	uint32_t oldstate = State();

	if (newstate != oldstate) {
		State = newstate;
		StateChanged(oldstate, newstate, reason);
	}
}

void Device::link_down()
{
	log_(0, "Link down, device is now disconnected");
	state(NM_DEVICE_STATE_DISCONNECTED, NM_DEVICE_STATE_REASON_UNKNOWN);
}

void Device::link_up(const boost::optional<mih::mac_addr> &poa)
{
	log_(0, "Link up");
	state(NM_DEVICE_STATE_IP_CONFIG, NM_DEVICE_STATE_REASON_UNKNOWN); // preparing to connect?
}

void Device::link_conf(const completion_handler &h,
                       const boost::optional<mih::network_id> &network,
                       const mih::configuration_list &lconf)
{
	log_(0, "Associating/Authenticating");

	_ctrl.link_conf(
		[h](mih::message &pm, const boost::system::error_code &ec) {
			mih::status st;
			pm >> mih::confirm(mih::confirm::link_conf)
				& mih::tlv_status(st);

			if (st == mih::status_success) {
				h(true);
			} else {
				h(false);
			}
		}, _lti, network, lconf);
}

void Device::l3_conf(const completion_handler &h,
                     const mih::ip_cfg_methods &cfg_methods,
                     const boost::optional<mih::ip_info_list> &address_list,
                     const boost::optional<mih::ip_info_list> &route_list,
                     const boost::optional<mih::ip_addr_list> &dns_list,
                     const boost::optional<mih::fqdn_list> &domain_list)
{
	log_(0, "Configuring L3");

	_ctrl.l3_conf(
		[h](mih::message &pm, const boost::system::error_code &ec) {
			mih::status st;
			pm >> mih::confirm(mih::confirm::l3_conf)
				& mih::tlv_status(st);

			if (st == mih::status_success) {
				h(true);
			} else {
				h(false);
			}
		}, _lti, cfg_methods, address_list, route_list, dns_list, domain_list);
}
