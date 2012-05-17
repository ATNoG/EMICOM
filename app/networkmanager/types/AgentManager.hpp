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

#ifndef NETWORKMANAGER_AGENTMANAGER__HPP_
#define NETWORKMANAGER_AGENTMANAGER__HPP_

#include "../dbus/adaptors/AgentManager.hpp"
#include <boost/noncopyable.hpp>
#include "odtone/logger.hpp"

namespace odtone {
namespace networkmanager {

class AgentManager : boost::noncopyable,
	public org::freedesktop::NetworkManager::AgentManager_adaptor,
	public DBus::IntrospectableAdaptor,
	public DBus::PropertiesAdaptor,
	public DBus::ObjectAdaptor
{
public:
	AgentManager(DBus::Connection &connection, const char *path);
	~AgentManager();

	void Unregister();
	void Register(const std::string& identifier);

private:
	std::string             _path;
	odtone::logger           log_;
};

}; };

#endif /* NETWORKMANAGER_AGENTMANAGER__HPP_ */
