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

#include "Connection.hpp"

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

using namespace odtone::networkmanager;

Connection::Connection(DBus::Connection &connection, const char* path, const boost::filesystem::path &file_path) :
	DBus::ObjectAdaptor(connection, path),
	_file_path(file_path),
	_path(path),
	log_(_path.c_str(), std::cout)
{
	read_settings();
}

Connection::Connection(DBus::Connection &connection, const char* path,
                       const settings_map &settings, const boost::filesystem::path &file_path) :
	DBus::ObjectAdaptor(connection, path),
	_file_path(file_path),
	_settings(settings),
	_path(path),
	log_(_path.c_str(), std::cout)
{
	// TODO calculate uuid
	write_settings();
}

Connection::~Connection()
{
}

Connection::settings_map Connection::GetSecrets(const std::string& setting_name)
{
	settings_map r;
	// TODO or empty?
	return r;
}

Connection::settings_map Connection::GetSettings()
{
	return _settings;
}

void Connection::Delete()
{
	// TODO we need a reference to the connections map!
}

void Connection::Update(const settings_map &properties)
{
	_settings = properties; // does this include uuid??
	write_settings();
}

std::string Connection::GetUuid()
{
	return _settings["connection"]["uuid"];
}

void Connection::read_settings()
{
	log_(0, "Loading connection at \"", _file_path.generic_string(), "\"");

	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini(_file_path.generic_string(), pt);

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
		_settings[v.first] = pairs;
	}
}

void Connection::write_settings()
{
}
