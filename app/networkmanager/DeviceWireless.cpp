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
#include "DeviceWireless_adaptor.h"

const char* const DeviceWireless::NAME = "org.freedesktop.NetworkManager21.Device.Wireless";
//const char* const DeviceWireless::PATH; // varies

DeviceWireless::DeviceWireless(DBus::Connection &connection, const char* path)
	: DBus::ObjectAdaptor(connection, path)
{
	// inherited from Device adaptor
	DeviceType = 0;
	FirmwareMissing = false;
	Managed = false;
	Dhcp6Config = "/";
	Ip6Config = "/";
	Dhcp4Config = "/";
	ActiveConnection = "/";
	State = 0;
	Ip4Address = 0;
	Capabilities = 0;
	Driver = "";
	IpInterface = "";
	Device_adaptor::Interface = ""; // Interface is ambiguous
	Udi = "";

	// inherited from Wireless adaptor
	WirelessCapabilities = 0;
	ActiveAccessPoint = "/";
	Bitrate = 0;
	Mode = 0;
	PermHwAddress = "00:11:22:33:44:55";
	HwAddress = "00:11:22:33:44:55";
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
