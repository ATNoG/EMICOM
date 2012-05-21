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

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <algorithm>

using namespace odtone::networkmanager;

Connection::Connection(DBus::Connection &connection,
                       const char* path,
                       std::map<DBus::Path, std::unique_ptr<Connection>> &container,
                       const boost::filesystem::path &file_path) :
	DBus::ObjectAdaptor(connection, path),
	_file_path(file_path),
	_container(container),
	_path(path),
	log_(_path.c_str(), std::cout)
{
	read_settings();
}

Connection::Connection(DBus::Connection &connection,
                       const char* path,
                       std::map<DBus::Path, std::unique_ptr<Connection>> &container,
                       const settings_map &settings,
                       const boost::filesystem::path &file_path) :
	DBus::ObjectAdaptor(connection, path),
	_file_path(file_path),
	_settings(settings),
	_container(container),
	_path(path),
	log_(_path.c_str(), std::cout)
{
	// TODO calculate uuid?
	write_settings();
}

Connection::~Connection()
{
}

Connection::settings_map Connection::GetSecrets(const std::string& setting_name)
{
	log_(0, "Getting connection secrets");

	settings_map r;
	// not supported

	log_(0, "Done");

	return r;
}

Connection::settings_map Connection::GetSettings()
{
	log_(0, "Getting connection settings");

	log_(0, "Done");

	return _settings;
}

void Connection::Delete()
{
	log_(0, "Deleting myself...");

	log_(0, "Removing file");
	boost::filesystem::remove(_file_path);

	log_(0, "Removing from container");
	_container.erase(_container.find(_path));
	// "this" doesn't exist, at this point!
}

void Connection::Update(const settings_map &properties)
{
	log_(0, "Updating settings locally");

	_settings = properties; // does this include uuid??

	write_settings();

	log_(0, "Done");
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

		setting_pairs pairs;
		BOOST_FOREACH (boost::property_tree::ptree::value_type &t, v.second) {
			std::string key = t.first;
			std::string value = t.second.get_value<std::string>();
			log_(0, "Adding pair \"", key, ":", value, "\"");

			::DBus::Variant val;

			// ssid and mac_address are not strings, but char arrays!
			if (key == "ssid") {
				::DBus::MessageIter ait = val.writer();
				ait << std::vector<uint8_t>(value.begin(), value.end());
			} else if (key == "mac-address") {
				std::vector<std::string> str_mac;
				boost::split(str_mac, value, boost::is_any_of(":"));

				// this array size should be 6, but with .size() no segfault occurs
				std::vector<uint8_t> mac(str_mac.size());
				std::transform(str_mac.begin(), str_mac.end(), mac.begin(),
					[](const std::string &s) {
						std::stringstream ss(s);
						ss << std::hex;
						int n;
						ss >> n;
						return n;
					});

				::DBus::MessageIter ait = val.writer();
				ait << mac;
			} else {
				val.writer().append_string(value.c_str());
			}

			pairs[key] = val;
		}
		_settings[v.first] = pairs;
	}
}

void Connection::write_settings()
{
	log_(0, "Persisting settings at \"", _file_path.generic_string(), "\"");

	boost::property_tree::ptree pt;

	BOOST_FOREACH (settings_map::value_type &v, _settings) {
		BOOST_FOREACH (setting_pairs::value_type &t, v.second) {
			std::string key = v.first + "." + t.first;
			std::string value = t.second;
			pt.put(key, value);
		}
	}

	boost::property_tree::ini_parser::write_ini(_file_path.generic_string(), pt);

	log_(0, "Done");
}
