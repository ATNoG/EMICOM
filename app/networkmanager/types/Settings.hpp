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
#include <boost/optional.hpp>
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
	::DBus::Path AddConnection(const settings_map &properties);
	::DBus::Path GetConnectionByUuid(const std::string &uuid);
	std::vector< ::DBus::Path > ListConnections();

	settings_map GetSettings(const DBus::Path &p);

	std::map<std::string, std::string> wpa_conf(const DBus::Path &connection);

	template <typename T>
	boost::optional<DBus::Path> get_connection_by_attribute(std::string setting, std::string attribute, T value)
	{
		boost::optional<DBus::Path> result;

		auto it = _connections.begin();
		while (!result && it != _connections.end()) {
			settings_map conn = it->second->GetSettings();

			// find setting
			auto set = conn.find(setting);
			if (set != conn.end()) {
				// find attribute
				auto att = set->second.find(attribute);
				if (att != set->second.end()) {
					// compare value
					DBus::Variant val = att->second;
					DBus::MessageIter mit = val.reader();
					T v;
					mit >> v;
					if (v == value) {
						result = it->first;
					}
				}
			}

			it ++;
		}

		return result;
	}

private:
	::DBus::Connection     &_connection;

	std::map<DBus::Path, std::shared_ptr<Connection>> _connections;
	unsigned int            _connection_counter;

	boost::filesystem::path _working_dir;
	std::string             _path;
	odtone::logger           log_;
};

}; };

#endif /* NETWORKMANAGER_SETTINGS__HPP_ */
