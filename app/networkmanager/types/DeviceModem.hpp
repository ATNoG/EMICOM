//==============================================================================
// Brief   : NetworkManager Device.Modem interface implementation
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

#ifndef NETWORKMANAGER_DEVICEMODEM__HPP_
#define NETWORKMANAGER_DEVICEMODEM__HPP_

#include "../dbus/adaptors/DeviceModem.hpp"
#include <boost/noncopyable.hpp>

#include "Device.hpp"

namespace odtone {
namespace networkmanager {

class DeviceModem :
	public Device,
	public org::freedesktop::NetworkManager::Device::Modem_adaptor
{
public:
	DeviceModem(DBus::Connection &connection, const char* path);
	~DeviceModem();

	// inherited from Device adaptor
	void Disconnect();
};

}; };

#endif /* NETWORKMANAGER_DEVICEMODEM__HPP_ */
