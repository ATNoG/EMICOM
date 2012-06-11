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

#ifndef NETWORKMANAGER_CONNECTION__HPP_
#define NETWORKMANAGER_CONNECTION__HPP_

#include "../dbus/adaptors/Connection.hpp"
#include <boost/noncopyable.hpp>
#include "odtone/logger.hpp"

#include <boost/filesystem.hpp>

#include "setting_types.hpp"

namespace odtone {
namespace networkmanager {

class Connection : boost::noncopyable,
	public org::freedesktop::NetworkManager::Settings::Connection_adaptor,
	public DBus::IntrospectableAdaptor,
	public DBus::ObjectAdaptor
{
public:
	Connection(DBus::Connection &connection,
	           const char* path,
               std::map<DBus::Path, std::shared_ptr<Connection>> &container,
	           const boost::filesystem::path &file_path);

	Connection(DBus::Connection &connection,
	           const char* path,
	           std::map<DBus::Path, std::shared_ptr<Connection>> &container,
	           const settings_map &settings,
	           const boost::filesystem::path &file_path);

	~Connection();

	settings_map GetSecrets(const std::string& setting_name);
	settings_map GetSettings();
	void Delete();
	void Update(const settings_map &properties);

	std::string GetUuid();

	std::map<std::string, std::string> supplicant_conf();

private:
	void read_settings();
	void write_settings();

private:
	boost::filesystem::path _file_path;
	settings_map            _settings;

	std::map<DBus::Path, std::shared_ptr<Connection>> &_container;

	DBus::Path              _path;
	odtone::logger           log_;
};

}; };

#endif /* NETWORKMANAGER_CONNECTION__HPP_ */
