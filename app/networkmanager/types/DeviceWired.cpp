//==============================================================================
// Brief   : NetworkManager Device.Wired interface implementation
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

#include "DeviceWired.hpp"

using namespace odtone::networkmanager;

DeviceWired::DeviceWired(DBus::Connection &connection, const char* path, mih_user &ctrl, mih::link_tuple_id &lti)
	: Device(connection, path, ctrl, lti)
{
	// FIXME
	// inherited from Device adaptor
	DeviceType = NM_DEVICE_TYPE_ETHERNET;
	FirmwareMissing = false;
	Managed = true;
	Dhcp6Config = "/";
	Ip6Config = "/";
	Dhcp4Config = "/";
	ActiveConnection = "/";

	State = NM_DEVICE_STATE_UNKNOWN; // TODO

	DBus::Struct<uint32_t, uint32_t> sr;
	sr._1 = State();
	sr._2 = NM_DEVICE_STATE_REASON_UNKNOWN;
	StateReason = sr;

	Ip4Address = 0;
	Capabilities = Device::NM_DEVICE_CAP_NM_SUPPORTED | Device::NM_DEVICE_CAP_CARRIER_DETECT;
	Driver = "odtone";
	IpInterface = "";
	Device_adaptor::Interface = ""; // Interface is ambiguous
	Udi = "";

	// inherited from Wired adaptor
	Carrier = true;
	Speed = 100;
	PermHwAddress = boost::get<mih::mac_addr>(_lti.addr).address();
	HwAddress = boost::get<mih::mac_addr>(_lti.addr).address();
}

DeviceWired::~DeviceWired()
{
}

void DeviceWired::link_down()
{
	log_(0, "Link down, device is now disconnected");
	state(NM_DEVICE_STATE_DISCONNECTED, NM_DEVICE_STATE_REASON_UNKNOWN);
}

void DeviceWired::link_up(const boost::optional<mih::mac_addr> &poa)
{
	log_(0, "Link up, device is now preparing L3 connectivity");
	//state(NM_DEVICE_STATE_IP_CONFIG, NM_DEVICE_STATE_REASON_UNKNOWN);
	state(NM_DEVICE_STATE_ACTIVATED, NM_DEVICE_STATE_REASON_UNKNOWN);
}
