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

#ifndef NETWORKMANAGER_DEVICEWIRELESS__HPP_
#define NETWORKMANAGER_DEVICEWIRELESS__HPP_

#include "../dbus/adaptors/DeviceWireless.hpp"

#include <boost/thread/shared_mutex.hpp>
#include "odtone/logger.hpp"

#include "Device.hpp"
#include "AccessPoint.hpp"

namespace odtone {
namespace networkmanager {

class DeviceWireless :
	public Device,
	public org::freedesktop::NetworkManager::Device::Wireless_adaptor
{

public:
	/**
	 * Enumeration of possible DeviceWireless Capabilities.
	 */
	enum NM_802_11_DEVICE_CAP {
		NM_802_11_DEVICE_CAP_NONE          = 0x0,
		NM_802_11_DEVICE_CAP_CIPHER_WEP40  = 0x1,
		NM_802_11_DEVICE_CAP_CIPHER_WEP104 = 0x2,
		NM_802_11_DEVICE_CAP_CIPHER_TKIP   = 0x4,
		NM_802_11_DEVICE_CAP_CIPHER_CCMP   = 0x8,
		NM_802_11_DEVICE_CAP_WPA           = 0x10,
		NM_802_11_DEVICE_CAP_RSN           = 0x20
	};

	/**
	 * Enumeration of possible DeviceWireless Modes.
	 */
	enum NM_802_11_MODE {
		NM_802_11_MODE_UNKNOWN = 0,
		NM_802_11_MODE_ADHOC   = 1,
		NM_802_11_MODE_INFRA   = 2
	};

public:

	/**
	 * Construct a new DeviceWireless D-Bus interface data type.
	 *
	 * @param connection @see odtone::networkmanager::Device::Device()
	 * @param path       @see odtone::networkmanager::Device::Device()
	 * @param ctrl       @see odtone::networkmanager::Device::Device()
	 * @param lti        @see odtone::networkmanager::Device::Device()
	 */
	DeviceWireless(DBus::Connection &connection,
	               const char* path,
	               mih_user &ctrl,
	               mih::link_tuple_id &lti);

	/**
	 * Destroy this object.
	 */
	~DeviceWireless();

	/**
	 * Get the list of associated AccessPoint objects.
	 *
	 * @return The D-Bus path list of AccessPoint objects.
	 */
	std::vector< ::DBus::Path > GetAccessPoints();

	/**
	 * @see Device::Disable()
	 */
	void Disable();

	/**
	 * @see Device::Disconnect()
	 */
	void Disconnect();

	/**
	 * @see Device::Enable()
	 */
	void Enable();

	/**
	 * Trigger a scan on this device.
	 */
	void Scan();

	/**
	 * Update the access point list.
	 *
	 * @param ldi A new or existing (update) AP info.
	 */
	void add_ap(mih::link_det_info ldi);

	/**
	 * Remove APs from the list last seen over a time duration.
	 *
	 * @param d AP minimum age for removal.
	 */
	void remove_aps_older_than(boost::posix_time::time_duration d);

	/**
	 * Get the info of the given access point.
	 *
	 * @param path The object's DBus path.
	 *
	 * @return the information of the access point.
	 */
	AccessPoint::bss_id get_access_point(const ::DBus::Path &path);

	/**
	 * Send a link_conf command to this device.
	 */
	void link_conf(const completion_handler &h,
	               const boost::optional<mih::link_addr> &poa,
	               const mih::configuration_list &lconf,
	               const DBus::Path &connection_active,
	               const DBus::Path &specific_object);


	/**
	 * Update parameters based on link reports.
	 *
	 * @param rpt_list The report list.
	 */
	void parameters_report(const mih::link_param_rpt_list &rpt_list);

protected:
	/**
	 * @see DBus::PropertyAdaptor
	 */
	// override from PropertiesAdaptor
	void on_get_property(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value);

	/**
	 * @see Device::state()
	 */
	void state(NM_DEVICE_STATE newstate, NM_DEVICE_STATE_REASON reason);

private:
	unsigned int     _access_point_count;

	boost::shared_mutex _access_points_map_mutex;
	std::map<DBus::Path, std::shared_ptr<AccessPoint>> _access_points_map;
};

}; };

#endif /* NETWORKMANAGER_DEVICEWIRELESS__HPP_ */
