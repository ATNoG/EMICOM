//==============================================================================
// Brief   : NetworkManager Device.Wired interface implementation
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

#ifndef NETWORKMANAGER_DBUS_DEVICEWIRED__HPP_
#define NETWORKMANAGER_DBUS_DEVICEWIRED__HPP_

#include "../adaptors/Device.hpp"
#include "../adaptors/DeviceWired.hpp"

namespace odtone {
namespace networkmanager {
namespace dbus {

class DeviceWired :
	public org::freedesktop::NetworkManager::Device_adaptor,
	public org::freedesktop::NetworkManager::Device::Wired_adaptor,
	public DBus::IntrospectableAdaptor,
	public DBus::PropertiesAdaptor,
	public DBus::ObjectAdaptor
{
public:
	DeviceWired(DBus::Connection &connection, const char* path);
	~DeviceWired();

	// inherited from Device adaptor
	void Disconnect();
};

}; }; };

#endif /* NETWORKMANAGER_DBUS_DEVICEWIRED__HPP_ */
