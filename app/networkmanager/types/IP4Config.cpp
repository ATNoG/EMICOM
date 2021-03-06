//==============================================================================
// Brief   : NetworkManager IP4Config interface implementation
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

#include "IP4Config.hpp"

using namespace odtone::networkmanager;

IP4Config::IP4Config(DBus::Connection &connection, const char* path)
	: DBus::ObjectAdaptor(connection, path)
{
	// FIXME
	Routes = std::vector< std::vector< uint32_t > >();
	Domains = std::vector< std::string >();
	WinsServers = std::vector< uint32_t >();
	Nameservers = std::vector< uint32_t >();
	Addresses = std::vector< std::vector< uint32_t > >();
}

IP4Config::~IP4Config()
{
}
