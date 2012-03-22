//==============================================================================
// Brief   : NetworkManager DHCP4Config interface implementation
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

#ifndef NETWORKMANAGER_DHCP4CONFIG__HPP_
#define NETWORKMANAGER_DHCP4CONFIG__HPP_

#include "../dbus/adaptors/DHCP4Config.hpp"
#include <boost/noncopyable.hpp>

namespace odtone {
namespace networkmanager {

class DHCP4Config : boost::noncopyable,
	public org::freedesktop::NetworkManager::DHCP4Config_adaptor,
	public DBus::IntrospectableAdaptor,
	public DBus::PropertiesAdaptor,
	public DBus::ObjectAdaptor
{
public:
	DHCP4Config(DBus::Connection &connection, const char* path);
	~DHCP4Config();
};

}; };

#endif /* NETWORKMANAGER_DHCP4CONFIG__HPP_ */
