//==============================================================================
// Brief   : NetworkManager AgentManager interface implementation
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

#ifndef NETWORKMANAGER_DBUS_AGENTMANAGER__HPP_
#define NETWORKMANAGER_DBUS_AGENTMANAGER__HPP_

#include "../adaptors/AgentManager.hpp"

namespace odtone {
namespace networkmanager {
namespace dbus {

class AgentManager :
	public org::freedesktop::NetworkManager::AgentManager_adaptor,
	public DBus::IntrospectableAdaptor,
	public DBus::PropertiesAdaptor,
	public DBus::ObjectAdaptor
{
public:
	static const char* const PATH;

	AgentManager(DBus::Connection &connection);
	~AgentManager();

	void Unregister();
	void Register(const std::string& identifier);
};

}; }; };

#endif /* NETWORKMANAGER_DBUS_AGENTMANAGER__HPP_ */
