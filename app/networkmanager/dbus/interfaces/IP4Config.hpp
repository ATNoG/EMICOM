//==============================================================================
// Brief   : NetworkManager IP4Config interface implementation
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

#ifndef NETWORKMANAGER_DBUS_IP4CONFIG__HPP_
#define NETWORKMANAGER_DBUS_IP4CONFIG__HPP_

#include "../adaptors/IP4Config.hpp"

namespace odtone {
namespace networkmanager {
namespace dbus {

class IP4Config :
	public org::freedesktop::NetworkManager::IP4Config_adaptor,
	public DBus::IntrospectableAdaptor,
	public DBus::PropertiesAdaptor,
	public DBus::ObjectAdaptor
{
public:
	IP4Config(DBus::Connection &connection, const char* path);
	~IP4Config();
};

}; }; };

#endif /* NETWORKMANAGER_DBUS_IP4CONFIG__HPP_ */