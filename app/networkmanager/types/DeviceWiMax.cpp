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

#include "DeviceWiMax.hpp"

using namespace odtone::networkmanager;

DeviceWiMax::DeviceWiMax(DBus::Connection &connection, const char* path, mih_user &ctrl, mih::link_tuple_id &lti)
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

	// inherited from WiMax adaptor
	HwAddress = boost::get<mih::mac_addr>(_lti.addr).address();
	CenterFrequency = 0;
	Rssi = 0;
	Cinr = 0;
	TxPower = 0;
	Bsid = boost::get<mih::mac_addr>(_lti.addr).address();
	ActiveNsp = "/";
}

DeviceWiMax::~DeviceWiMax()
{
}

std::vector< ::DBus::Path > DeviceWiMax::GetNspList()
{
	std::vector< ::DBus::Path > r;
	// TODO
	return r;
}

void DeviceWiMax::link_down()
{
	// TODO
	// check if device is up, or just disconnected
}

void DeviceWiMax::link_up(const boost::optional<mih::mac_addr> &poa)
{
	// TODO
	// change state
}
