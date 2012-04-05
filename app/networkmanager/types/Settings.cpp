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
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace boost {
	void assertion_failed_msg(char const* a, char const* b, char const* c, char const* d, long e)
	{
		// nothing
	}
};

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
			Connection::settings_map set = read_connection(*dir_it);

			std::stringstream ss;
			ss << NetworkManager::PATH << "/Connections/" << ++_connection_counter;
			::DBus::Path connection_path = ss.str();
			std::unique_ptr<Connection> connection(new Connection(_connection, connection_path.c_str(), set));
			_connections[connection_path] = std::move(connection);
		}
		++ dir_it;
	}

	// FIXME
	CanModify = false;
	Hostname = "";
}

Settings::~Settings()
{
}

void Settings::SaveHostname(const std::string& hostname)
{
	// TODO
}

::DBus::Path Settings::AddConnection(const Connection::settings_map &properties)
{
	log_(0, "Adding new connection");

	// TODO calculate uuid!
	std::stringstream ss;
	ss << NetworkManager::PATH << "/Connections/" << ++_connection_counter;
	::DBus::Path connection_path = ss.str();
	std::unique_ptr<Connection> connection(new Connection(_connection, connection_path.c_str(), properties));
	_connections[connection_path] = std::move(connection);

	// TODO persist?

	log_(0, "Done");
	return connection_path;
}

::DBus::Path Settings::GetConnectionByUuid(const std::string& uuid)
{
	::DBus::Path r;

	// TODO

	return r;
}

std::vector< ::DBus::Path > Settings::ListConnections()
{
	std::vector< ::DBus::Path > r;

	std::map<DBus::Path, std::unique_ptr<Connection>>::iterator it = _connections.begin();
	while (it != _connections.end()) {
		r.push_back(it->first);
		it++;
	}

	return r;
}

Connection::settings_map Settings::read_connection(const boost::filesystem::path &path)
{
	log_(0, "Loading connection at \"", path.generic_string(), "\"");

	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini(path.generic_string(), pt);

	Connection::settings_map m;
	BOOST_FOREACH (boost::property_tree::ptree::value_type &v, pt) {
		std::string group = v.first;
		log_(0, "Parsing group \"", group, "\"");

		Connection::setting_pairs pairs;
		BOOST_FOREACH (boost::property_tree::ptree::value_type &t, v.second) {
			std::string key = t.first;
			std::string value = t.second.get_value<std::string>();
			log_(0, "Adding pair \"", key, ":", value, "\"");

			::DBus::Variant val;
			val.writer().append_string(value.c_str());

			pairs[key] = val;
			
		}
		m[v.first] = pairs;
	}

	return m;
}
