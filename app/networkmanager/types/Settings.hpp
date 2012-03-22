//==============================================================================
// Brief   : NetworkManager Settings interface implementation
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

#ifndef NETWORKMANAGER_SETTINGS__HPP_
#define NETWORKMANAGER_SETTINGS__HPP_

#include "../dbus/adaptors/Settings.hpp"
#include <boost/noncopyable.hpp>

namespace odtone {
namespace networkmanager {

class Settings : boost::noncopyable,
	public org::freedesktop::NetworkManager::Settings_adaptor,
	public DBus::IntrospectableAdaptor,
	public DBus::PropertiesAdaptor,
	public DBus::ObjectAdaptor
{
public:
	Settings(DBus::Connection &connection, const char* path);
	~Settings();

	void SaveHostname(const std::string& hostname);
	::DBus::Path AddConnection(const std::map< std::string, std::map< std::string, ::DBus::Variant > >& connection);
	::DBus::Path GetConnectionByUuid(const std::string& uuid);
	std::vector< ::DBus::Path > ListConnections();
};

}; };

#endif /* NETWORKMANAGER_SETTINGS__HPP_ */
