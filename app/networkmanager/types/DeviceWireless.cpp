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
	: Device(connection, path, ctrl, lti)
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

void DeviceWireless::Disable()
{
	Device::Disable();

	ActiveAccessPoint = "/";
	PropertiesChanged(map_list_of("ActiveAccessPoint", to_variant(ActiveAccessPoint()))
	                             ("ActiveConnection",  to_variant(ActiveConnection())));
}

void DeviceWireless::Disconnect()
{
	Device::Disconnect();

	ActiveAccessPoint = "/";
	PropertiesChanged(map_list_of("ActiveAccessPoint", to_variant(ActiveAccessPoint()))
	                             ("ActiveConnection",  to_variant(ActiveConnection())));
}

void DeviceWireless::Enable()
{
	{
		boost::unique_lock<boost::shared_mutex> lock(_access_points_map_mutex);
		remove_aps_older_than(boost::posix_time::seconds(0));
	}

	log_(0, "Enabling");

	// assume success
	state(NM_DEVICE_STATE_DISCONNECTED, NM_DEVICE_STATE_REASON_UNKNOWN);

	_ctrl.power_up(
		[&](mih::message &pm, const boost::system::error_code &ec) {
			mih::status st;
			pm >> mih::confirm(mih::confirm::link_actions)
				& mih::tlv_status(st);

			if (st != mih::status_success) {
				state(NM_DEVICE_STATE_UNKNOWN, NM_DEVICE_STATE_REASON_UNKNOWN);
			} else {
				RequestScan(std::map<std::string, DBus::Variant>());
			}
		}, _lti, false);
}

void DeviceWireless::RequestScan(const std::map<std::string, DBus::Variant> &options)
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

void DeviceWireless::link_conf(const completion_handler &h,
                               const boost::optional<mih::link_addr> &poa,
                               const mih::configuration_list &lconf,
                               const DBus::Path &connection_active,
                               const DBus::Path &specific_object)
{
	ActiveAccessPoint = specific_object;

	Device::link_conf(h, poa, lconf, connection_active, specific_object);

	PropertiesChanged(map_list_of("ActiveAccessPoint", to_variant(ActiveAccessPoint()))
	                             ("ActiveConnection",  to_variant(ActiveConnection())));
}

void DeviceWireless::parameters_report(const mih::link_param_rpt_list &rpt_list)
{
	log_(0, "Handling parameters report");

	for (auto it = rpt_list.begin(); it != rpt_list.end(); ++it) {
		mih::link_param_type lpt = it->param.type;
		mih::link_param_val  v   = boost::get<mih::link_param_val>(it->param.value);

		// PoA RSSI
		mih::link_param_802_11 *param_802_11 = boost::get<mih::link_param_802_11>(&lpt);
		if (param_802_11 && (*param_802_11 == mih::link_param_802_11_rssi)) {
			boost::shared_lock<boost::shared_mutex> lock(_access_points_map_mutex);
			auto active_ap_it = _access_points_map.find(ActiveAccessPoint());
			if (active_ap_it != _access_points_map.end()) {
				mih::sig_strength value = static_cast<odtone::sint8>(v);
				active_ap_it->second->update_strength(value);
			}
		}

		// PoA link data rate
		mih::link_param_gen *param_gen = boost::get<mih::link_param_gen>(&lpt);
		if (param_gen && (*param_gen == mih::link_param_gen_data_rate)) {
			Bitrate = v;
			PropertiesChanged(map_list_of("Bitrate", to_variant(Bitrate())));
		}
	}
}

void DeviceWireless::on_get_property(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value)
{
	PropertiesAdaptor::on_get_property(interface, property, value);
}

void DeviceWireless::add_ap(mih::link_det_info ldi)
{
	boost::unique_lock<boost::shared_mutex> lock(_access_points_map_mutex);

	// Get PoA's MAC address
	mih::mac_addr poa_addr = boost::get<mih::mac_addr>(boost::get<mih::link_addr>(ldi.id.poa_addr));

	// Generate D-Bus path
	std::stringstream path_str;
	path_str << _dbus_path << "/AccessPoints/" << boost::algorithm::erase_all_copy(poa_addr.address(), ":");
	DBus::Path path_dbus = path_str.str();

	// If it exists, update, else add new
	auto it = _access_points_map.find(path_dbus);
	if (it != _access_points_map.end()) {
		it->second->Update(ldi);
	} else {
		log_(0, "Adding AP ", path_str.str(), " with SSID ", ldi.network_id);

		_access_points_map[path_dbus] = std::shared_ptr<AccessPoint>(
			new AccessPoint(_connection, path_str.str().c_str(), ldi));

		// announce addition
		Wireless_adaptor::AccessPointAdded(path_dbus);
	}

	// TODO make this configurable
	// clean older scan results
	remove_aps_older_than(boost::posix_time::seconds(30));
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
