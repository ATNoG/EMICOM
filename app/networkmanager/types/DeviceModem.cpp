//==============================================================================
// Brief   : NetworkManager Device.Modem interface implementation
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

#include "DeviceModem.hpp"

using namespace odtone::networkmanager;

DeviceModem::DeviceModem(DBus::Connection &connection, const char* path, mih_user &ctrl, mih::link_tuple_id &lti)
	: Device(connection, path, ctrl, lti)
{
	// FIXME
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

	// inherited from Wired adaptor
	ModemCapabilities = 0;
	CurrentCapabilities = 0;
}

DeviceModem::~DeviceModem()
{
}

void DeviceModem::link_down()
{
	// TODO
	// check if device is up, or just disconnected
}

void DeviceModem::link_up(const boost::optional<mih::mac_addr> &poa)
{
	// TODO
	// change state
}
