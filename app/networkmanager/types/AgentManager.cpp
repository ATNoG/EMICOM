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

using namespace odtone::networkmanager;

AgentManager::AgentManager(DBus::Connection &connection, const char *path) :
	DBus::ObjectAdaptor(connection, path),
	_path(path),
	log_(_path.c_str(), std::cout)
{
}

AgentManager::~AgentManager()
{
}

void AgentManager::Unregister()
{
	log_(0, "Unregistering");
	// TODO dummy
}

void AgentManager::Register(const std::string& identifier)
{
	log_(0, "Registering agent ", identifier);
	// TODO dummy
}
