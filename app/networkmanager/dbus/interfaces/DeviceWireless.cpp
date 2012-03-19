//==============================================================================
// Brief   : NetworkManager Device.Wireless interface implementation
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

#include "DeviceWireless.hpp"

using namespace odtone::networkmanager::dbus;

DeviceWireless::DeviceWireless(DBus::Connection &connection, const char* path, if_80211 &fi)
	: DBus::ObjectAdaptor(connection, path)
{
	// FIXME
	// inherited from Device adaptor
	DeviceType = Device::NM_DEVICE_TYPE_WIFI; 
	FirmwareMissing = false; // by definition, if it got here...
	Managed = true;          // by definition...

	Dhcp6Config = "/";       // TODO
	Ip6Config = "/";         // TODO
	Dhcp4Config = "/";       // TODO
	ActiveConnection = "/";  // TODO

	State = Device::NM_DEVICE_STATE_UNKNOWN;
	Capabilities = Device::NM_DEVICE_CAP_NM_SUPPORTED;

	Ip4Address = 0;                 // TODO
	Driver = "";                    // TODO
	IpInterface = "";               // TODO
	Device_adaptor::Interface = ""; // TODO
	Udi = "";                       // TODO

	// inherited from Wireless adaptor
	WirelessCapabilities = 0; // TODO
	ActiveAccessPoint = "/";  // TODO
	Bitrate = 0;              // TODO
	Mode = NM_802_11_MODE_INFRA; // or NM_802_11_MODE_UNKNOWN ?
	PermHwAddress = fi.mac_address().address(); //"00:11:22:33:44:55";
	HwAddress = fi.mac_address().address(); //"00:11:22:33:44:55";
}

DeviceWireless::~DeviceWireless()
{
}

void DeviceWireless::Disconnect()
{
	// TODO
}

std::vector< ::DBus::Path > DeviceWireless::GetAccessPoints()
{
	std::vector< ::DBus::Path > r;
	// TODO
	return r;
}
