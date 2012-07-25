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
#include "util.hpp"

#include <boost/assign/list_of.hpp>

using namespace odtone::networkmanager;
using namespace boost::assign;

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

void DeviceWired::Disable()
{
	Device::Disable();

	PropertiesChanged(map_list_of("ActiveConnection", to_variant(ActiveConnection())));
}

void DeviceWired::Disconnect()
{
	Device::Disconnect();

	PropertiesChanged(map_list_of("ActiveConnection", to_variant(ActiveConnection())));
}

void DeviceWired::link_conf(const completion_handler &h,
                            const boost::optional<mih::link_addr> &poa,
                            const mih::configuration_list &lconf,
                            const DBus::Path &connection_active,
                            const DBus::Path &specific_object)
{
	Device::link_conf(h, poa, lconf, connection_active, specific_object);

	PropertiesChanged(map_list_of("ActiveConnection", to_variant(ActiveConnection())));
}

void DeviceWired::state(NM_DEVICE_STATE newstate, NM_DEVICE_STATE_REASON reason)
{
	Device::state(newstate, reason);

	PropertiesChanged(map_list_of("State",       to_variant(State()))
	                             ("StateReason", to_variant(StateReason())));
}
