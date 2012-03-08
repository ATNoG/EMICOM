//==============================================================================
// Brief   : NetworkManager Application
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

#include "NetworkManager.hpp"
#include "AccessPoint.hpp"
#include "DeviceWireless.hpp"
#include "DeviceWired.hpp"
#include "IP4Config.hpp"
#include "DHCP4Config.hpp"
#include "DHCP6Config.hpp"
#include "Settings.hpp"
#include "Connection.hpp"
#include "ConnectionActive.hpp"
#include "AgentManager.hpp"

#include <dbus-c++/dbus.h>
#include <cstdlib> // for EXIT_{SUCCESS,FAILURE}

int main(int argc, char *argv[])
{
	DBus::BusDispatcher dispatcher;
	DBus::default_dispatcher = &dispatcher;

	// setup NetworkManager
	DBus::Connection conn = DBus::Connection::SystemBus();
	conn.request_name(NetworkManager::NAME);

	AgentManager agent(conn);
	NetworkManager manager(conn);

	// setup an example AccessPoint
	AccessPoint point(conn, "/org/freedesktop/NetworkManager21/AccessPoint/0");

	// setup an example DeviceWireless
	DeviceWireless wireless(conn, "/org/freedesktop/NetworkManager21/Devices/0");

	// setup an example DeviceWired
	DeviceWired wired(conn, "/org/freedesktop/NetworkManager21/Devices/1");

	// setup an example IP4Config
	IP4Config config4(conn, "/org/freedesktop/NetworkManager21/IP4Config/0");

	// setup an example DHCP4Config
	DHCP4Config configd4(conn, "/org/freedesktop/NetworkManager21/DHCP4Config/0");

	// Setup an example DHCP6Config
	DHCP6Config configd6(conn, "/org/freedesktop/NetworkManager21/DHCP6Config/0");

	// setup an example Settings
	Settings tings(conn, "/org/freedesktop/NetworkManager21/Settings");

	// setup an example Connection Settings
	Connection ection(conn, "/org/freedesktop/NetworkManager21/Settings/0");

	// setup an example Connection.Active
	ConnectionActive active(conn, "/org/freedesktop/NetworkManager21/ActiveConnections/0");

	dispatcher.enter();

	return EXIT_SUCCESS;
}
