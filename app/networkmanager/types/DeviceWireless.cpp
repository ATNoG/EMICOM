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
#include "util.hpp"

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <cstring>
#include <exception>

using namespace odtone::networkmanager;

DeviceWireless::DeviceWireless(DBus::Connection &connection,
                               const char* path,
                               mih_user &ctrl,
                               mih::link_tuple_id &lti)
	: Device(connection, path, ctrl, lti), _connection(connection), _access_point_count(0)
{
	// FIXME
	// inherited from Device adaptor
	Device_adaptor::DeviceType      = NM_DEVICE_TYPE_WIFI; // by definition
	Device_adaptor::FirmwareMissing = false;
	Device_adaptor::Managed         = true;

	Device_adaptor::Dhcp6Config = "/";       // TODO
	Device_adaptor::Ip6Config = "/";         // TODO
	Device_adaptor::Dhcp4Config = "/";       // TODO
	Device_adaptor::ActiveConnection = "/";  // TODO

	State = NM_DEVICE_STATE_UNKNOWN; // TODO

	Device_adaptor::Capabilities = NM_DEVICE_CAP_NM_SUPPORTED;

	Device_adaptor::Driver = "odtone";
	Device_adaptor::IpInterface = "";   // TODO
	Device_adaptor::Interface = "";     // TODO
	Device_adaptor::Udi = "";           // TODO

	// inherited from Wireless adaptor
	Wireless_adaptor::WirelessCapabilities = 0; // TODO
	Wireless_adaptor::ActiveAccessPoint = "/";  // TODO
	Wireless_adaptor::PermHwAddress = boost::get<mih::mac_addr>(_lti.addr).address();
	Wireless_adaptor::HwAddress = boost::get<mih::mac_addr>(_lti.addr).address();
	Wireless_adaptor::Bitrate = 0; // TODO no support for bitrates yet
	Wireless_adaptor::Mode = NM_802_11_MODE_INFRA;
}

DeviceWireless::~DeviceWireless()
{
}

std::vector< ::DBus::Path > DeviceWireless::GetAccessPoints()
{
	std::vector< ::DBus::Path > r;

	boost::shared_lock<boost::shared_mutex> lock(_access_points_map_mutex);

	auto it = _access_points_map.begin();
	while (it != _access_points_map.end()) {
		r.push_back(it->first);
		it++;
	}

	return r;
}

void DeviceWireless::Enable()
{
	{
		boost::unique_lock<boost::shared_mutex> lock(_access_points_map_mutex);
		remove_aps_older_than(boost::posix_time::seconds(0));
	}

	log_(0, "Enabling, with scan request");

	// assume success
	state(NM_DEVICE_STATE_ACTIVATED, NM_DEVICE_STATE_REASON_UNKNOWN);

	_ctrl.power_up(
		[&](mih::message &pm, const boost::system::error_code &ec) {
			mih::status st;
			pm >> mih::confirm(mih::confirm::link_actions)
				& mih::tlv_status(st);

			if (st != mih::status_success) {
				state(NM_DEVICE_STATE_DISCONNECTED, NM_DEVICE_STATE_REASON_UNKNOWN);
			}
		}, _lti, true);
}

void DeviceWireless::Scan()
{
	log_(0, "Scanning");

	_ctrl.scan(
		[](mih::message &pm, const boost::system::error_code &ec) {
			// link detected events will update the ap list
		}, _lti);
}

void DeviceWireless::property(const std::string &property, const DBus::Variant &value)
{
	DBus::MessageIter it = value.reader();

	if (boost::iequals(property, "ActiveAccessPoint")) {
		DBus::Path p;
		it >> p;
		ActiveAccessPoint = p;
	} else if (boost::iequals(property, "HwAddress")) {
		std::string s;
		it >> s;
		HwAddress = s;
	} else if (boost::iequals(property, "PermHwAddress")) {
		std::string s;
		it >> s;
		PermHwAddress = s;
	} else if (boost::iequals(property, "Mode")) {
		uint8_t u;
		it >> u;
		Mode = u;
	} else if (boost::iequals(property, "Bitrate")) {
		uint8_t u;
		it >> u;
		Bitrate = u;
	} else if (boost::iequals(property, "WirelessCapabilities")) {
		uint8_t u;
		it >> u;
		WirelessCapabilities = u;
	}

	std::map<std::string, DBus::Variant> m;
	m[property] = value;
	PropertiesChanged(m);
}

void DeviceWireless::link_down()
{
	log_(0, "Link down, device is now disconnected");
	state(NM_DEVICE_STATE_DISCONNECTED, NM_DEVICE_STATE_REASON_UNKNOWN);
	DBus::Path no_active_access_point = "/";
	property("ActiveAccessPoint", to_variant(no_active_access_point));
}

void DeviceWireless::link_up(const boost::optional<mih::mac_addr> &poa)
{
	log_(0, "Link up");
	// todo use poa for ActiveAccessPoint...
	state(NM_DEVICE_STATE_ACTIVATED, NM_DEVICE_STATE_REASON_UNKNOWN);
	//state(NM_DEVICE_STATE_IP_CONFIG, NM_DEVICE_STATE_REASON_UNKNOWN); // preparing to connect?
}

void DeviceWireless::on_get_property(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
	// TODO bypass to wpa_supplicant?

	PropertiesAdaptor::on_get_property(interface, property, value);
}

void DeviceWireless::add_ap(mih::link_det_info ldi)
{
	boost::unique_lock<boost::shared_mutex> lock(_access_points_map_mutex);

	// TODO make this configurable
	// clean older scan results
	remove_aps_older_than(boost::posix_time::seconds(30));

	// if it exists in the list, update
	auto map_it = _access_points_map.begin();
	while (map_it != _access_points_map.end()) {
		std::string map_addr = map_it->second->HwAddress();
		mih::mac_addr poa_addr = boost::get<mih::mac_addr>(boost::get<mih::link_addr>(ldi.id.poa_addr));

		if (boost::iequals(map_addr, poa_addr.address())) {
			// update AP
			map_it->second->Update(ldi);

			return;
		}
		map_it++;
	}

	// not found
	// add AccessPoint to the list
	std::stringstream path_str;
	path_str << _dbus_path << "/AccessPoints/" << ++_access_point_count;

	DBus::Path path_dbus = path_str.str();
	_access_points_map[path_dbus] = std::shared_ptr<AccessPoint>(
		new AccessPoint(_connection, path_str.str().c_str(), ldi));

	log_(0, "Adding AP ", path_str.str(), " with SSID ", ldi.network_id);

	// announce addition
	Wireless_adaptor::AccessPointAdded(path_dbus);
}

void DeviceWireless::remove_aps_older_than(boost::posix_time::time_duration d)
{
	auto map_it = _access_points_map.begin();
	while (map_it != _access_points_map.end()) {
		if (map_it->second->last_change() > d) {
			Wireless_adaptor::AccessPointRemoved(map_it->first);

			map_it = _access_points_map.erase(map_it);
		} else {
			map_it++;
		}
	}
}

AccessPoint::bss_id DeviceWireless::get_access_point(const ::DBus::Path &path)
{
	boost::shared_lock<boost::shared_mutex> lock(_access_points_map_mutex);

	auto bss = _access_points_map.find(path);
	if (bss != _access_points_map.end()) {
		return bss->second->get_id();
	}

	throw std::runtime_error("No such ap");
}
