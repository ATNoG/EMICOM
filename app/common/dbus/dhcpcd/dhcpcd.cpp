//==============================================================================
// Brief   : dhcpcd main D-Bus datatype proxy
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

#include "dhcpcd.hpp"

namespace odtone {
namespace dhcp {

dhcpcd::dhcpcd(DBus::Connection &connection, const char *path, const char *name)
	: DBus::ObjectProxy(connection, path, name)
{
}

void dhcpcd::Event(const std::map< std::string, ::DBus::Variant >& configuration)
{
	std::cerr << "Event" << std::endl;
}

void dhcpcd::StatusChanged(const std::string& status)
{
	std::cerr << "StatusChanged: " << status << std::endl;
}

void dhcpcd::ScanResultsSignal(const std::string& interface)
{
	std::cerr << "ScanResults" << std::endl;
}

}; };
