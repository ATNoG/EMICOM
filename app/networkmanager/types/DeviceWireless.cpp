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
#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/assign/list_of.hpp>
#include <cstring>
#include <exception>

using namespace odtone::networkmanager;
using namespace boost::assign;

DeviceWireless::DeviceWireless(DBus::Connection &connection,
                               const char* path,
                               mih_user &ctrl,
                               mih::link_tuple_id &lti)
	: Device(connection, path, ctrl, lti), _access_point_count(0)
{
	// FIXME
	// inherited from Device adaptor
	DeviceType      = NM_DEVICE_TYPE_WIFI; // by definition
	FirmwareMissing = false;
	Managed         = true;

	Dhcp6Config = "/";       // TODO
	Ip6Config = "/";         // TODO
	Dhcp4Config = "/";       // TODO
	Ip4Config = "/";         // TODO
	ActiveConnection = "/";

	State = NM_DEVICE_STATE_UNKNOWN; // TODO

	DBus::Struct<uint32_t, uint32_t> sr;
	sr._1 = State();
	sr._2 = NM_DEVICE_STATE_REASON_UNKNOWN;
	StateReason = sr;

	Capabilities = NM_DEVICE_CAP_NM_SUPPORTED;

	Driver = "odtone";
	IpInterface = "";   // TODO
	Device_adaptor::Interface = "";     // TODO
	Udi = "";           // TODO

	// inherited from Wireless adaptor
	WirelessCapabilities = 0; // TODO
	ActiveAccessPoint = "/";
	PermHwAddress = boost::get<mih::mac_addr>(_lti.addr).address();
	HwAddress = boost::get<mih::mac_addr>(_lti.addr).address();
	Bitrate = 0; // TODO no support for bitrates yet
	Mode = NM_802_11_MODE_INFRA;
}

DeviceWireless::~DeviceWireless()
{
}

std::vector< ::DBus::Path > DeviceWireless::GetAccessPoints()
{
	std::vector< ::DBus::Path > r;

	boost::shared_lock<boost::shared_mutex> lock(_access_points_map_mutex);

	boost::copy(_access_points_map | boost::adaptors::map_keys, std::back_inserter(r));

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
	state(NM_DEVICE_STATE_DISCONNECTED, NM_DEVICE_STATE_REASON_UNKNOWN);

	_ctrl.power_up(
		[&](mih::message &pm, const boost::system::error_code &ec) {
			mih::status st;
			pm >> mih::confirm(mih::confirm::link_actions)
				& mih::tlv_status(st);

			//if (st != mih::status_success) {
			//	state(NM_DEVICE_STATE_UNKNOWN, NM_DEVICE_STATE_REASON_UNKNOWN);
			//}
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

void DeviceWireless::state(NM_DEVICE_STATE newstate, NM_DEVICE_STATE_REASON reason)
{
	Device::state(newstate, reason);

	PropertiesChanged(map_list_of("State",       to_variant(State()))
	                             ("StateReason", to_variant(StateReason())));
}

void DeviceWireless::link_down()
{
	Device::link_down();

	ActiveAccessPoint = "/";
	PropertiesChanged(map_list_of("ActiveAccessPoint", to_variant(ActiveAccessPoint()))
	                             ("ActiveConnection",  to_variant(ActiveConnection())));
}

void DeviceWireless::link_conf(const completion_handler &h,
                               const boost::optional<mih::link_addr> &poa,
                               const mih::configuration_list &lconf,
                               const DBus::Path &connection_active,
                               const DBus::Path &specific_object)
{
	ActiveAccessPoint = specific_object;
	ActiveConnection = connection_active;

	PropertiesChanged(map_list_of("ActiveAccessPoint", to_variant(ActiveAccessPoint()))
	                             ("ActiveConnection",  to_variant(ActiveConnection())));

	Device::link_conf(h, poa, lconf, connection_active, specific_object);
}

void DeviceWireless::on_get_property(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
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
