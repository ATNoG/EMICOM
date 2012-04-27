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
#include <boost/algorithm/string.hpp>
#include <cstring>

using namespace odtone::networkmanager;

DeviceWireless::DeviceWireless(DBus::Connection &connection,
                               const char* path,
                               mih_user &ctrl,
                               odtone::mih::mac_addr &addr)
	: Device(connection, path, ctrl), _connection(connection), _access_point_count(0)
{
	_lti.addr = addr;
	_lti.type = mih::link_type_802_11;

	// FIXME
	// inherited from Device adaptor
	Device_adaptor::DeviceType      = NM_DEVICE_TYPE_WIFI; // by definition
	Device_adaptor::FirmwareMissing = false; 
	Device_adaptor::Managed         = true;

	Device_adaptor::Dhcp6Config = "/";       // TODO
	Device_adaptor::Ip6Config = "/";         // TODO
	Device_adaptor::Dhcp4Config = "/";       // TODO
	Device_adaptor::ActiveConnection = "/";  // TODO

	Device_adaptor::State = NM_DEVICE_STATE_ACTIVATED; // TODO

	Device_adaptor::Capabilities = NM_DEVICE_CAP_NM_SUPPORTED;

	Device_adaptor::Driver = "nl80211";             // by design
	Device_adaptor::IpInterface = "wlan0";			// TODO
	Device_adaptor::Interface = "wlan0";            // TODO
	Device_adaptor::Udi = "";                       // TODO

	// inherited from Wireless adaptor
	Wireless_adaptor::WirelessCapabilities = 0; // TODO
	Wireless_adaptor::ActiveAccessPoint = "/";  // TODO
	Wireless_adaptor::PermHwAddress = addr.address();
	Wireless_adaptor::HwAddress = addr.address();
	Wireless_adaptor::Bitrate = 0; // TODO no support for bitrates yet
	Wireless_adaptor::Mode = NM_802_11_MODE_INFRA;
}

DeviceWireless::~DeviceWireless()
{
}

std::vector< ::DBus::Path > DeviceWireless::GetAccessPoints()
{
	std::vector< ::DBus::Path > r;

	auto it = _access_points_map.begin();
	while (it != _access_points_map.end()) {
		r.push_back(it->first);
		it++;
	}

	return r;
}

void DeviceWireless::Scan()
{
	log_(0, "Scanning");

	_ctrl.scan(_lti,
		[&](mih::message &pm, const boost::system::error_code &ec) {
			// TODO update state
		});
}

void DeviceWireless::link_down()
{
	// TODO
	// request state, if powered down or just disconnected
}

void DeviceWireless::link_up(const odtone::mih::mac_addr &poa)
{
	log_(0, "Link up, device is now preparing to connect");
	state(NM_DEVICE_STATE_CONFIG, NM_DEVICE_STATE_REASON_UNKNOWN); // preparing to connect?
}

void DeviceWireless::on_get_property(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
	// TODO bypass to wpa_supplicant?

	PropertiesAdaptor::on_get_property(interface, property, value);
}

void DeviceWireless::refresh_accesspoint_list(std::vector<mih::link_det_info> ldil)
{
	// TODO
	// have a list of elements with timeouts
/*	bool found;
	auto map_it = _access_points_map.begin();
	while (map_it != _access_points_map.end()) {
		std::string map_addr = map_it->second->HwAddress();

		found = false;

		auto poa_it = ldil.begin();
		while (poa_it != ldil.end() && !found) {
			mih::mac_addr poa_addr_ = boost::get<mih::mac_addr>(boost::get<mih::link_addr>(poa_it->id.poa_addr));
			std::string poa_addr = poa_addr_.address();

			if (boost::iequals(map_addr, poa_addr)) {
				// update AP
				map_it->second->Update(*poa_it);

				// already in the map, remove from list
				poa_it = ldil.erase(poa_it);
				found = true;
			} else {
				poa_it++;
			}
		}

		if (!found) {
			// announce removal
			Wireless_adaptor::AccessPointRemoved(map_it->first);

			// not in range anymore, so remove from map
			map_it = _access_points_map.erase(map_it);
		} else {
			map_it++;
		}
	}

	// add remaining AccessPoints (new in range) to the map
	auto poa_it = ldil.begin();
	while (poa_it != ldil.end()) {
		std::stringstream path_str;
		path_str << _dbus_path << "/AccessPoints/" << ++_access_point_count;

		DBus::Path path_dbus = path_str.str();
		_access_points_map[path_dbus] = std::unique_ptr<AccessPoint>(
			new AccessPoint(_connection, path_str.str().c_str(), *poa_it));

		// announce addition
		Wireless_adaptor::AccessPointAdded(path_dbus);
		poa_it++;
	}*/
}
