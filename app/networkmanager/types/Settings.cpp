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

using namespace odtone::networkmanager;

Settings::Settings(DBus::Connection &connection, const char* path)
	: DBus::ObjectAdaptor(connection, path)
{
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

::DBus::Path Settings::AddConnection(const std::map< std::string, std::map< std::string, ::DBus::Variant > >& connection)
{
	::DBus::Path r;
	// TODO
	return r;
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
	// TODO
	return r;
}