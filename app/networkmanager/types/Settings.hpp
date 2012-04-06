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
#include "odtone/logger.hpp"

#include "Connection.hpp"
#include <boost/filesystem.hpp>

namespace odtone {
namespace networkmanager {

class Settings : boost::noncopyable,
	public org::freedesktop::NetworkManager::Settings_adaptor,
	public DBus::IntrospectableAdaptor,
	public DBus::PropertiesAdaptor,
	public DBus::ObjectAdaptor
{
public:
	Settings(DBus::Connection &connection, const char *path, const char *working_dir);
	~Settings();

	void SaveHostname(const std::string &hostname);
	::DBus::Path AddConnection(const Connection::settings_map &properties);
	::DBus::Path GetConnectionByUuid(const std::string &uuid);
	std::vector< ::DBus::Path > ListConnections();

private:
	::DBus::Connection     &_connection;

	std::map<DBus::Path, std::unique_ptr<Connection>> _connections;
	unsigned int            _connection_counter;
	
	boost::filesystem::path _working_dir;
	std::string             _path;
	odtone::logger           log_;
};

}; };

#endif /* NETWORKMANAGER_SETTINGS__HPP_ */
