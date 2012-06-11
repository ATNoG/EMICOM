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

#ifndef NETWORKMANAGER_CONNECTIONACTIVE__HPP_
#define NETWORKMANAGER_CONNECTIONACTIVE__HPP_

#include "../dbus/adaptors/ConnectionActive.hpp"
#include <boost/noncopyable.hpp>

namespace odtone {
namespace networkmanager {

class ConnectionActive : boost::noncopyable,
	public org::freedesktop::NetworkManager::Connection::Active_adaptor,
	public DBus::IntrospectableAdaptor,
	public DBus::PropertiesAdaptor,
	public DBus::ObjectAdaptor
{
public:

	/**
	 * Enumeration of possible Connection.Active states.
	 */
	enum NM_ACTIVE_CONNECTION_STATE {
		NM_ACTIVE_CONNECTION_STATE_UNKNOWN      = 0,
		NM_ACTIVE_CONNECTION_STATE_ACTIVATING   = 1,
		NM_ACTIVE_CONNECTION_STATE_ACTIVATED    = 2,
		NM_ACTIVE_CONNECTION_STATE_DEACTIVATING = 3
	};

public:
	ConnectionActive(DBus::Connection &connection,
	                 const char* path,
	                 const ::DBus::Path &_connection,
	                 const ::DBus::Path &_specific_object,
	                 const std::string &_uuid,
	                 const std::vector< ::DBus::Path > &_devices,
	                 uint32_t _state,
	                 bool _default,
	                 bool _default6,
	                 bool _vpn);
	~ConnectionActive();

	void state(NM_ACTIVE_CONNECTION_STATE s);
};

}; };

#endif /* NETWORKMANAGER_CONNECTIONACTIVE__HPP_ */
