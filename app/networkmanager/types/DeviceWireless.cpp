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
	Device_adaptor::Ip4Config = "/";         // TODO
	Device_adaptor::ActiveConnection = "/";

	State = NM_DEVICE_STATE_UNKNOWN; // TODO

	Device_adaptor::Capabilities = NM_DEVICE_CAP_NM_SUPPORTED;

	Device_adaptor::Driver = "odtone";
	Device_adaptor::IpInterface = "";   // TODO
	Device_adaptor::Interface = "";     // TODO
	Device_adaptor::Udi = "";           // TODO

	// inherited from Wireless adaptor
	Wireless_adaptor::WirelessCapabilities = 0; // TODO
	Wireless_adaptor::ActiveAccessPoint = "/";
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

	for (auto it = _access_points_map.begin(); it != _access_points_map.end(); ++it) {
		r.push_back(it->first);
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

template <class T>
void DeviceWireless::property(const std::string &property, const T &value)
{
	std::map<std::string, DBus::Variant> props;
	props[property] = to_variant(value);
	PropertiesChanged(props);
}

void DeviceWireless::link_down()
{
	Device::link_down();

	ActiveAccessPoint = "/";
	property("ActiveAccessPoint", ActiveAccessPoint());
}

void DeviceWireless::link_up(const boost::optional<mih::mac_addr> &poa)
{
	Device::link_up(poa);

	// set the active access point
	if (!poa) {
		return;
	}

	boost::shared_lock<boost::shared_mutex> lock(_access_points_map_mutex);

	for (auto it = _access_points_map.begin(); it != _access_points_map.end(); ++ it) {
		if (boost::iequals(it->second->HwAddress(), poa.get().address())) {
			ActiveAccessPoint = it->first;
			property("ActiveAccessPoint", ActiveAccessPoint());
			it = _access_points_map.end();
		}
	}
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
	for (auto it = _access_points_map.begin(); it != _access_points_map.end(); ++it) {
		std::string map_addr = it->second->HwAddress();
		mih::mac_addr poa_addr = boost::get<mih::mac_addr>(boost::get<mih::link_addr>(ldi.id.poa_addr));

		if (boost::iequals(map_addr, poa_addr.address())) {
			// update AP
			it->second->Update(ldi);

			return;
		}
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
	for (auto it = _access_points_map.begin(); it != _access_points_map.end(); ) {
		if (it->second->last_change() > d) {
			Wireless_adaptor::AccessPointRemoved(it->first);

			it = _access_points_map.erase(it);
		} else {
			++it;
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

	throw std::runtime_error("No such ap, " + path);
}
