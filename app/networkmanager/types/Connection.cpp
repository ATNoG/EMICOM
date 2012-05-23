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

#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

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

settings_map Connection::GetSecrets(const std::string& setting_name)
{
	log_(0, "Getting connection secrets");

	settings_map r;
	// not supported

	log_(0, "Done");

	return r;
}

settings_map Connection::GetSettings()
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

	// signal removal
	Removed();

	log_(0, "Removing from container");
	_container.erase(_container.find(_path));
	// "this" doesn't exist, at this point!
}

void Connection::Update(const settings_map &properties)
{
	log_(0, "Updating settings locally");

	// thecnically the map will be different once reloaded again from file!
	_settings = properties;
	write_settings();

	// signal changes
	Updated();

	log_(0, "Done");
}

std::string Connection::GetUuid()
{
	return _settings["connection"]["uuid"];
}

void Connection::read_settings()
{
	log_(0, "Loading connection at \"", _file_path.generic_string(), "\"");

	connection_settings s;

	std::ifstream ifs(_file_path.generic_string());
	boost::archive::text_iarchive ia(ifs);
	ia >> s;

	_settings = s.to_map();
}

void Connection::write_settings()
{
	log_(0, "Persisting settings at \"", _file_path.generic_string(), "\"");

	connection_settings s(_settings);

	std::ofstream ofs(_file_path.generic_string());
	boost::archive::text_oarchive oa(ofs);
	oa << s;

	log_(0, "Done");
}
