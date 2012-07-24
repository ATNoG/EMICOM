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
#include "util.hpp"

using namespace odtone::networkmanager;

ConnectionActive::ConnectionActive(DBus::Connection &connection,
                                   const char* path,
                                   const ::DBus::Path &_connection,
                                   const ::DBus::Path &_specific_object,
                                   const std::string &_uuid,
                                   const std::vector< ::DBus::Path > &_devices,
                                   uint32_t _state,
                                   bool _default,
                                   bool _default6,
                                   bool _vpn)
	: DBus::ObjectAdaptor(connection, path)
{
	Connection     = _connection;
	SpecificObject = _specific_object;
	Uuid           = _uuid;
	Devices        = _devices;
	State          = _state;
	Default        = _default;
	Default6       = _default6;
	Vpn            = _vpn;
}

ConnectionActive::~ConnectionActive()
{
}

void ConnectionActive::state(NM_ACTIVE_CONNECTION_STATE s)
{
	if (s != State()) {
		State = s;

		std::map<std::string, ::DBus::Variant> m;
		m["State"] = to_variant(State());

		PropertiesChanged(m);
	}
}
