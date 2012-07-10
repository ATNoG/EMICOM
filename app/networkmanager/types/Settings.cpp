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

#include "Settings.hpp"

#include "NetworkManager.hpp"

using namespace odtone::networkmanager;

Settings::Settings(DBus::Connection &connection, const char *path, const char *working_dir) :
	DBus::ObjectAdaptor(connection, path),
	_connection(connection),
	_connection_counter(0),
	_working_dir(working_dir),
	_path(path),
	log_(_path.c_str(), std::cout)
{
	// if it doesn't exist, try to create
	if (!boost::filesystem::exists(working_dir)) {
		if (!boost::filesystem::create_directory(working_dir)) {
			throw "Error creating settings directory";
		}
	} else if (!boost::filesystem::is_directory(working_dir)) {
		throw "Path exists but is not a directory";
	}

	// load the files in the folder
	boost::filesystem::directory_iterator dir_it(_working_dir);
	boost::filesystem::directory_iterator end_it;
	while (dir_it != end_it) {
		if (boost::filesystem::is_regular_file(boost::filesystem::status(*dir_it))) {
			std::stringstream ss;
			ss << _path << "/" << ++_connection_counter;

			::DBus::Path connection_path = ss.str();
			std::shared_ptr<Connection> connection(
				new Connection(_connection, connection_path.c_str(), _connections, *dir_it));
			_connections[connection_path] = connection;
		}
		++ dir_it;
	}

	// FIXME
	CanModify = true;
	Hostname = "";
}

Settings::~Settings()
{
}

void Settings::SaveHostname(const std::string& hostname)
{
	// TODO
}

::DBus::Path Settings::AddConnection(const settings_map &properties)
{
	log_(0, "Adding new connection");

	try {
		std::string filename = properties.find("connection")->second.find("uuid")->second;
		std::string file_path_str = _working_dir.generic_string() + "/" + filename;
		boost::filesystem::path file_path(file_path_str);

		std::stringstream ss;
		ss << _path << "/" << ++_connection_counter;
		::DBus::Path connection_path = ss.str();
		std::shared_ptr<Connection> connection(
			new Connection(_connection, connection_path.c_str(), _connections, properties, file_path));
		_connections[connection_path] = connection;

		log_(0, "Done");
		return connection_path;
	} catch (...) {
		log_(0, "Exception occurred while storing settings");
		throw DBus::Error("org.freedesktop.NetworkManager.Error.ConnectionInvalid",
		                  "The specified settings are invalid");
	}
}

::DBus::Path Settings::GetConnectionByUuid(const std::string& uuid)
{
	log_(0, "Getting connection by UUID");

	try {
		auto it = _connections.begin();
		while (it != _connections.end()) {
			if(it->second->GetUuid() == uuid) {
				log_(0, "Connection found at \"", it->first, "\"");
				return it->first;
			}
			it++;
		}
	} catch (...) {
		// TODO the connection probably has no uuid, so it should be deleted...
	}

	log_(0, "No such connection");
	throw DBus::Error("org.freedesktop.NetworkManager.Error.UnknownConnection",
	                  "Couldn't find a connection with given uuid");
}

std::vector< ::DBus::Path > Settings::ListConnections()
{
	log_(0, "Getting connection list");

	std::vector< ::DBus::Path > r;

	auto it = _connections.begin();
	while (it != _connections.end()) {
		r.push_back(it->first);
		it++;
	}

	log_(0, "Done");

	return r;
}

settings_map Settings::GetSettings(const DBus::Path &p)
{
	log_(0, "Getting connection at \"", p, "\"");

	auto it = _connections.find(p);
	if (it != _connections.end()) {
		return it->second->GetSettings();
	}

	throw DBus::Error("org.freedesktop.NetworkManager.Error.UnknownConnection",
	                  "Couldn't find a connection at given Path");
}

std::map<std::string, std::string> Settings::wpa_conf(const DBus::Path &connection)
{
	auto it = _connections.find(connection);
	if (it != _connections.end()) {
		return it->second->supplicant_conf();
	}

	throw std::runtime_error("No such connection");
}
