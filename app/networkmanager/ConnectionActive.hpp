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

#ifndef CONNECTIONACTIVE__HPP_
#define CONNECTIONACTIVE__HPP_

#include "ConnectionActive_adaptor.h"

class ConnectionActive :
	public org::freedesktop::NetworkManager::Connection::Active_adaptor,
	public DBus::IntrospectableAdaptor,
	public DBus::PropertiesAdaptor,
	public DBus::ObjectAdaptor
{
public:
	ConnectionActive(DBus::Connection &connection, const char* path);
	~ConnectionActive();
};

#endif /* CONNECTIONACTIVE__HPP_ */
