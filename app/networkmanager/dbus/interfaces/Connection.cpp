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

using namespace odtone::networkmanager::dbus;

Connection::Connection(DBus::Connection &connection, const char* path)
	: DBus::ObjectAdaptor(connection, path)
{
}

Connection::~Connection()
{
}

std::map< std::string, std::map< std::string, ::DBus::Variant > > Connection::GetSecrets(const std::string& setting_name)
{
	std::map< std::string, std::map< std::string, ::DBus::Variant > > r;
	// TODO
	return r;
}

std::map< std::string, std::map< std::string, ::DBus::Variant > > Connection::GetSettings()
{
	std::map< std::string, std::map< std::string, ::DBus::Variant > > r;
	// TODO
	return r;
}

void Connection::Delete()
{
}

void Connection::Update(const std::map< std::string, std::map< std::string, ::DBus::Variant > >& properties)
{
}
