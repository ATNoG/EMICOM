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

#include "dbus/nm-dbus.hpp"
#include "driver/if_80211.hpp"

#include <dbus-c++/dbus.h>
#include <cstdlib> // for EXIT_{SUCCESS,FAILURE}

using namespace odtone;

int main(int argc, char *argv[])
{
	DBus::BusDispatcher dispatcher;
	DBus::default_dispatcher = &dispatcher;

	// setup NetworkManager
	DBus::Connection conn = DBus::Connection::SystemBus();
	conn.request_name(networkmanager::dbus::NetworkManager::NAME);

	networkmanager::dbus::AgentManager agent(conn);
	networkmanager::dbus::NetworkManager manager(conn);

	// setup an example AccessPoint
	networkmanager::dbus::AccessPoint point(conn, "/org/freedesktop/NetworkManager21/AccessPoint/0");

	// setup an example DeviceWireless
	boost::asio::io_service ios;
	odtone::mih::mac_addr mac;
	mac.address("00:27:10:7d:5f:30");

	if_80211 fi(ios, mac);
	networkmanager::dbus::DeviceWireless wireless(conn, "/org/freedesktop/NetworkManager21/Devices/0", fi);

	// setup an example DeviceWired
	networkmanager::dbus::DeviceWired wired(conn, "/org/freedesktop/NetworkManager21/Devices/1");

	// setup an example DeviceModem
	networkmanager::dbus::DeviceModem modem(conn, "/org/freedesktop/NetworkManager21/Devices/2");

	// setup an example DeviceWiMax
	networkmanager::dbus::DeviceWiMax wimax(conn, "/org/freedesktop/NetworkManager21/Devices/3");

	// setup an example WiMaxNsp
	networkmanager::dbus::WiMaxNsp nsp(conn, "/org/freedesktop/NetworkManager21/Nsp/0");

	// setup an example IP4Config
	networkmanager::dbus::IP4Config config4(conn, "/org/freedesktop/NetworkManager21/IP4Config/0");

	// setup an example IP6Config
	networkmanager::dbus::IP6Config config6(conn, "/org/freedesktop/NetworkManager21/IP6Config/0");

	// setup an example DHCP4Config
	networkmanager::dbus::DHCP4Config configd4(conn, "/org/freedesktop/NetworkManager21/DHCP4Config/0");

	// Setup an example DHCP6Config
	networkmanager::dbus::DHCP6Config configd6(conn, "/org/freedesktop/NetworkManager21/DHCP6Config/0");

	// setup an example Settings
	networkmanager::dbus::Settings tings(conn, "/org/freedesktop/NetworkManager21/Settings");

	// setup an example Connection Settings
	networkmanager::dbus::Connection ection(conn, "/org/freedesktop/NetworkManager21/Settings/0");

	// setup an example Connection.Active
	networkmanager::dbus::ConnectionActive active(conn, "/org/freedesktop/NetworkManager21/ActiveConnections/0");

	dispatcher.enter();

	return EXIT_SUCCESS;
}
