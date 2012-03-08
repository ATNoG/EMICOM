//==============================================================================
// Brief   : NetworkManager D-Bus interface implementation
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

const char* const NetworkManager::NAME = "org.freedesktop.NetworkManager21";
const char* const NetworkManager::PATH = "/org/freedesktop/NetworkManager21";

NetworkManager::NetworkManager(DBus::Connection &connection)
	: DBus::ObjectAdaptor(connection, PATH)
{
	// FIXME
	State = 0;
	Version = "0.0";
	ActiveConnections = std::vector< ::DBus::Path >();
	WimaxHardwareEnabled = false;
	WimaxEnabled = false;
	WwanHardwareEnabled = false;
	WwanEnabled = false;
	WirelessHardwareEnabled = false;
	WirelessEnabled = false;
	NetworkingEnabled = false;
}

NetworkManager::~NetworkManager()
{
}

uint32_t NetworkManager::state()
{
	// TODO
	return 0;
}

void NetworkManager::SetLogging(const std::string& level, const std::string& domains)
{
	// TODO
}

std::map< std::string, std::string > NetworkManager::GetPermissions()
{
	std::map< std::string, std::string > r;
	// TODO
	return r;
}

void NetworkManager::Enable(const bool& enable)
{
	// TODO
}

void NetworkManager::Sleep(const bool& sleep)
{
	// TODO
}

void NetworkManager::DeactivateConnection(const ::DBus::Path& active_connection)
{
	// TODO
}

void NetworkManager::AddAndActivateConnection(
	const std::map< std::string, std::map< std::string, ::DBus::Variant > >& connection,
	const ::DBus::Path& device,
	const ::DBus::Path& specific_object,
	::DBus::Path& path, ::DBus::Path& active_connection)
{
	// TODO
}

::DBus::Path NetworkManager::ActivateConnection(
	const ::DBus::Path& connection,
	const ::DBus::Path& device,
	const ::DBus::Path& specific_object)
{
	::DBus::Path r;
	// TODO
	return r;
}

::DBus::Path NetworkManager::GetDeviceByIpIface(const std::string& iface)
{
	::DBus::Path r;
	// TODO
	return r;
}

std::vector< ::DBus::Path > NetworkManager::GetDevices()
{
	std::vector< ::DBus::Path > r;
	// TODO
	return r;
}
