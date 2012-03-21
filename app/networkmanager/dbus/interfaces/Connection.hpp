//==============================================================================
// Brief   : NetworkManager Connection interface implementation
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

#ifndef NETWORKMANAGER_DBUS_CONNECTION__HPP_
#define NETWORKMANAGER_DBUS_CONNECTION__HPP_

#include "../adaptors/Connection.hpp"

namespace odtone {
namespace networkmanager {
namespace dbus {

class Connection :
	public org::freedesktop::NetworkManager::Settings::Connection_adaptor,
	public DBus::IntrospectableAdaptor,
	public DBus::ObjectAdaptor
{
public:
	Connection(DBus::Connection &connection, const char* path);
	~Connection();

	std::map< std::string, std::map< std::string, ::DBus::Variant > > GetSecrets(const std::string& setting_name);
	std::map< std::string, std::map< std::string, ::DBus::Variant > > GetSettings();
	void Delete();
	void Update(const std::map< std::string, std::map< std::string, ::DBus::Variant > >& properties);
};

}; }; };

#endif /* NETWORKMANAGER_DBUS_CONNECTION__HPP_ */
