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

#include "AgentManager.hpp"

const char* const AgentManager::PATH = "/org/freedesktop/NetworkManager21/AgentManager";

AgentManager::AgentManager(DBus::Connection &connection)
	: DBus::ObjectAdaptor(connection, PATH)
{
}

AgentManager::~AgentManager()
{
	// TODO
}

void AgentManager::Unregister()
{
	// TODO
}

void AgentManager::Register(const std::string& identifier)
{
	// TODO
}
