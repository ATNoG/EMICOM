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

#include <dbus-c++/dbus.h>
#include <cstdlib> // for EXIT_{SUCCESS,FAILURE}

int main(int argc, char *argv[])
{
	DBus::BusDispatcher dispatcher;
	DBus::default_dispatcher = &dispatcher;

	DBus::Connection conn = DBus::Connection::SystemBus();
	conn.request_name(NetworkManager::NAME);

	NetworkManager manager(conn);

	dispatcher.enter();

	return EXIT_SUCCESS;
}
