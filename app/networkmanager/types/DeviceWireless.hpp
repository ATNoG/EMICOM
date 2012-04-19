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
#include "odtone/logger.hpp"

#include "Device.hpp"
#include "AccessPoint.hpp"
#include "../driver/if_80211.hpp"

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
	 * @param connection The D-Bus (system) connection.
	 * @param path The D-Bus path of this object.
	 * @param address The MAC Address of the underlying device.
	 */
	DeviceWireless(DBus::Connection &connection, const char* path, odtone::mih::mac_addr &address);

	/**
	 * Destroy this object.
	 */
	~DeviceWireless();

	/**
	 * @see Device::Disconnect()
	 */
	void Disconnect();

	/**
	 * @see Device::Enable()
	 */
	void Enable();

	/**
	 * Get the list of associated AccessPoint objects.
	 *
	 * @return The D-Bus path list of AccessPoint objects.
	 */
	std::vector< ::DBus::Path > GetAccessPoints();

	/**
	 * Refresh the access point list.
	 * Invoke this when new scan results show up.
	 */
	void refresh_accesspoint_list();

	/**
	 * see Device::link_down()
	 */
	void link_down();

	/**
	 * see Device::link_up()
	 */
	void link_up(const odtone::mih::mac_addr &poa);

protected:
	// override from PropertiesAdaptor
	/**
	 * @see DBus::PropertyAdaptor
	 */
	void on_get_property(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value);

private:
	/**
	 * Auxiliary function to compare access points.
	 *
	 * @param ap The AccessPoint to compare with.
	 * @param poa The poa_info to compare with.
	 *
	 * @return True if they share the same mac address, false otherwise.
	 */
	bool same_access_point(AccessPoint &ap, poa_info &poa);

private:
	DBus::Connection &_connection;
	if_80211          _fi;

	unsigned int     _access_point_count;

	std::string      _path;
	odtone::logger   log_;

	std::map<DBus::Path, std::unique_ptr<AccessPoint>> _access_points_map;
};

}; };

#endif /* NETWORKMANAGER_DEVICEWIRELESS__HPP_ */
