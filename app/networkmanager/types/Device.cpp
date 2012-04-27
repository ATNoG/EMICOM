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

Device::Device(DBus::Connection &connection, const char* path, mih_user &ctrl)
	: DBus::ObjectAdaptor(connection, path), _ctrl(ctrl), _dbus_path(path), log_(_dbus_path.c_str(), std::cout)
{
}

Device::~Device()
{
}

void Device::Disconnect()
{
	log_(0, "Disconnecting");

	_ctrl.disconnect(_lti,
		[&](mih::message &pm, const boost::system::error_code &ec) {
			// TODO update state
		});
}

void Device::Enable()
{
	log_(0, "Enabling");

	_ctrl.power_up(_lti,
		[&](mih::message &pm, const boost::system::error_code &ec) {
			// TODO update state
		});
}

void Device::Disable()
{
	log_(0, "Disabling");
	_ctrl.power_down(_lti,
		[&](mih::message &pm, const boost::system::error_code &ec) {
			// TODO update state
		});
}

void Device::state(NM_DEVICE_STATE newstate, NM_DEVICE_STATE_REASON reason)
{
	uint32_t oldstate = State();
	State = newstate;

	StateChanged(oldstate, newstate, reason);
}
