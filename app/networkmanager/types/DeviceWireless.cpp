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

#include <boost/foreach.hpp>

using namespace odtone::networkmanager;

DeviceWireless::DeviceWireless(DBus::Connection &connection, const char* path, odtone::mih::mac_addr &address) :
	Device(connection, path),
	_fi(address),
	_path(path),
	log_(_path.c_str(), std::cout)
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

	Driver = "nl80211";             // by design
	IpInterface = _fi.ifname();
	Device_adaptor::Interface = ""; // TODO
	Udi = "";                       // TODO

	// inherited from Wireless adaptor
	WirelessCapabilities = 0; // TODO
	ActiveAccessPoint = "/";  // TODO
	PermHwAddress = _fi.mac_address().address(); //"00:11:22:33:44:55";
	HwAddress = _fi.mac_address().address(); //"00:11:22:33:44:55";

	// store ipv4address, in decimal format
	BOOST_FOREACH (const boost::asio::ip::address &addr, _fi.addresses()) {
		if (addr.is_v4()) {
			// in reverse byte order!
			std::array<unsigned char, 4> bytev4 = addr.to_v4().to_bytes();
			std::reverse(bytev4.begin(), bytev4.end());

			boost::asio::ip::address_v4 reversev4(bytev4);
			Ip4Address = reversev4.to_ulong();

			// store the first only
			break;
		}
	}
}

DeviceWireless::~DeviceWireless()
{
}

void DeviceWireless::Disconnect()
{
	log_(0, "Disconnecting");
	try {
		_fi.set_op_mode(odtone::mih::link_ac_type_power_down);
		state(NM_DEVICE_STATE_DISCONNECTED, NM_DEVICE_STATE_REASON_UNKNOWN); // TODO better reasons?
	} catch (...) {
		log_(0, "Exception occurred, potentially not disconnected");
	}
}

std::vector< ::DBus::Path > DeviceWireless::GetAccessPoints()
{
	log_(0, "Getting Access Points");
	std::vector< ::DBus::Path > r;

	std::map<DBus::Path, AccessPoint>::iterator it = _access_points_map.begin();
	while (it != _access_points_map.end()) {
		r.push_back(it->first);
		it++;
	}

	log_(0, "Done");

	return r;
}

void DeviceWireless::on_get_property(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
	if (property == "Bitrate") {
		try {
			Bitrate = _fi.link_bitrate();
		} catch(...) {
			Bitrate = 0;
		}
	} else if (property == "Mode") {
		if_80211::if_type t = _fi.get_if_type();
		switch (t) {
		case if_80211::if_type::adhoc:
			Mode = NM_802_11_MODE_ADHOC;
			break;
		case if_80211::if_type::station:
			Mode = NM_802_11_MODE_INFRA;
			break;
		default:
			Mode = NM_802_11_MODE_UNKNOWN;
		}
	}

	PropertiesAdaptor::on_get_property(interface, property, value);
}
