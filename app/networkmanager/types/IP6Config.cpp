//==============================================================================
// Brief   : NetworkManager IP6Config interface implementation
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

#include "IP6Config.hpp"

using namespace odtone::networkmanager;

IP6Config::IP6Config(DBus::Connection &connection, const char* path)
	: DBus::ObjectAdaptor(connection, path)
{
	// FIXME
	Routes = std::vector< ::DBus::Struct< std::vector< uint8_t >, uint32_t, std::vector< uint8_t >, uint32_t > >();
	Domains = std::vector< std::string >();
	Nameservers = std::vector< std::vector< uint8_t > >();
	Addresses = std::vector< ::DBus::Struct< std::vector< uint8_t >, uint32_t, std::vector< uint8_t > > >();
}

IP6Config::~IP6Config()
{
}
