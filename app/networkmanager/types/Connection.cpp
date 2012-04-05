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

using namespace odtone::networkmanager;

Connection::Connection(DBus::Connection &connection, const char* path, const settings_map &settings) :
	DBus::ObjectAdaptor(connection, path),
	_settings(settings)
{
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
	// TODO
	// Bummer, we need a reference from the parent!
}

void Connection::Update(const settings_map &properties)
{
	_settings = properties;
	// TODO
	// This, too has to be redirected to the Settings object.
}
