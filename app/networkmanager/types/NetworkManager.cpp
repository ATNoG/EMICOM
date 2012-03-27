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

using namespace odtone::networkmanager;

const char* const NetworkManager::NAME = "org.freedesktop.NetworkManager21";
const char* const NetworkManager::PATH = "/org/freedesktop/NetworkManager21";

NetworkManager::NetworkManager(DBus::Connection &connection) :
	DBus::ObjectAdaptor(connection, PATH), _connection(connection),
	log_(PATH, std::cout)
{
	// FIXME
	State = NM_STATE_UNKNOWN;
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
	return State();
}

void NetworkManager::state(NM_STATE newstate)
{
	// store
	State = newstate;

	// signal
	StateChanged(newstate);
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
	NetworkingEnabled = enable;

	if (!enable) {
		std::vector<std::unique_ptr<Device>>::iterator it = _device_list.begin();
		while (it != _device_list.end()) {
			(*it)->Disconnect();
			it++;
		}
	} else {
		// TODO trigger a "start managing"
	}
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

void NetworkManager::add_802_11_device(odtone::mih::mac_addr &address)
{
	log_(0, "Adding WiFi device, address: ", address.address());

	try {
		// this is just to get the deviceindex, for a nicer D-Bus Device path
		if_80211 fi(address);

		std::stringstream path;
		path << PATH << "/Devices/" << fi.ifindex();
		std::unique_ptr<Device> d(new DeviceWireless(_connection, path.str().c_str(), address));

		// if networking is disabled, shut this interface
		if (!NetworkingEnabled()) {
			d->Disconnect();
		} else {
			// TODO
			// attempt to connect? if disconnected?
		}

		// save the device
		_device_list.push_back(std::move(d));

		// signal new device
		DeviceAdded(path.str().c_str());
		log_(0, "Device added");
	} catch (...) {
		log_(0, "Error adding device");
	}
}
