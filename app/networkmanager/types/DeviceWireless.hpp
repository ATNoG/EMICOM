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
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>
#include "odtone/logger.hpp"

#include "Device.hpp"
#include "AccessPoint.hpp"

namespace odtone {
namespace networkmanager {

class DeviceWireless :
// NetworkManager D-Bus
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
	 * Override Enable method to power up and scan in the same action message.
	 */
	void Enable();

	/**
	 * Trigger a scan on this device.
	 */
	void Scan();

	/**
	 * see Device::link_down()
	 */
	void link_down();

	/**
	 * see Device::link_up()
	 */
	void link_up(const odtone::mih::mac_addr &poa);

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

protected:
	/**
	 * @see DBus::PropertyAdaptor
	 */
	// override from PropertiesAdaptor
	void on_get_property(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value);

private:
	DBus::Connection &_connection;

	unsigned int     _access_point_count;

	boost::shared_mutex _access_points_map_mutex;
	std::map<DBus::Path, std::unique_ptr<AccessPoint>> _access_points_map;
};

}; };

#endif /* NETWORKMANAGER_DEVICEWIRELESS__HPP_ */
