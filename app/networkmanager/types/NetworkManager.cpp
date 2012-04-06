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

NetworkManager::NetworkManager(DBus::Connection &connection, const char *dbus_path, const char *settings_path) :
	DBus::ObjectAdaptor(connection, dbus_path),
	_connection(connection),
	_dbus_path(dbus_path),
	_settings_path(settings_path),
	_settings(_connection, _dbus_path.append("/Settings").c_str(), settings_path),
	log_(_dbus_path.c_str(), std::cout)
{
	// FIXME
	State = NM_STATE_UNKNOWN;
	Version = "0.0";
	ActiveConnections = std::vector< ::DBus::Path >();
	WimaxHardwareEnabled = false;
	WimaxEnabled = false;
	WwanHardwareEnabled = false;
	WwanEnabled = false;
	WirelessHardwareEnabled = true;
	WirelessEnabled = true;
	NetworkingEnabled = true;
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
		log_(0, "Disabling");

		state(NM_STATE_DISCONNECTING);

		std::map<DBus::Path, std::unique_ptr<Device>>::iterator it = _device_map.begin();
		while (it != _device_map.end()) {
			it->second->Disconnect();
			it++;
		}

		state(NM_STATE_DISCONNECTED);
	} else {
		log_(0, "Enabling");
		// TODO trigger a "start managing"
	}
}

void NetworkManager::Sleep(const bool& sleep)
{
	if (sleep) {
		log_(0, "Going to sleep");

		state(NM_STATE_DISCONNECTING);

		std::map<DBus::Path, std::unique_ptr<Device>>::iterator it = _device_map.begin();
		while (it != _device_map.end()) {
			it->second->Disconnect();
			it++;
		}

		state(NM_STATE_ASLEEP);
	} else {
		log_(0, "Waking up");
		// TODO trigger a "start managing"
	}
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
	log_(0, "Getting device by name (IpIface): ", iface);

	std::map<DBus::Path, std::unique_ptr<Device>>::iterator it = _device_map.begin();
	while (it != _device_map.end()) {
		if (it->second->IpInterface() == iface) {
			log_(0, iface, " found");
			return it->first;
		}
		it++;
	}

	log_(0, iface, " not found");
	throw DBus::Error("org.freedesktop.NetworkManager.UnknownDevice", "No device found for the requested iface.");
}

std::vector< ::DBus::Path > NetworkManager::GetDevices()
{
	log_(0, "Getting all devices");

	std::vector< ::DBus::Path > r;

	std::map<DBus::Path, std::unique_ptr<Device>>::iterator it = _device_map.begin();
	while (it != _device_map.end()) {
		r.push_back(it->first);
		it++;
	}

	log_(0, "Done");
	return r;
}

void NetworkManager::add_802_11_device(odtone::mih::mac_addr &address)
{
	log_(0, "Adding WiFi device, address: ", address.address());

	try {
		// this is just to get the deviceindex, for a nicer D-Bus Device path
		if_80211 fi(address);

		std::stringstream path;
		path << _dbus_path << "/Devices/" << fi.ifindex();
		std::unique_ptr<Device> d(new DeviceWireless(_connection, path.str().c_str(), address));

		// if networking is disabled, shut this interface
		if (!NetworkingEnabled()) {
			d->Disconnect();
		} else {
			// TODO
			// attempt to connect? if disconnected?
		}

		// save the device
		_device_map[DBus::Path(path.str())] = std::move(d);

		// signal new device
		DeviceAdded(path.str().c_str());
		log_(0, "Device added");
	} catch (...) {
		log_(0, "Error adding device");
	}
}
