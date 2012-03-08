//==============================================================================
// Brief   : NetworkManager DHCP6Config interface implementation
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

#include "DHCP6Config.hpp"

DHCP6Config::DHCP6Config(DBus::Connection &connection, const char* path)
	: DBus::ObjectAdaptor(connection, path)
{
	Options = std::map< std::string, ::DBus::Variant >();
}

DHCP6Config::~DHCP6Config()
{
}
