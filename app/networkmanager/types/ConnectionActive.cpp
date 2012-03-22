//==============================================================================
// Brief   : NetworkManager Connection.Active interface implementation
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

#include "ConnectionActive.hpp"

using namespace odtone::networkmanager;

ConnectionActive::ConnectionActive(DBus::Connection &connection, const char* path)
	: DBus::ObjectAdaptor(connection, path)
{
	// FIXME
	Vpn = false;
	Default6 = false;
	Default = false;
	State = 0;
	Devices = std::vector< ::DBus::Path >();
	Uuid = "";
	SpecificObject = "/";
	Connection = "/";
}

ConnectionActive::~ConnectionActive()
{
}
