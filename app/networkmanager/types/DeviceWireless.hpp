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
#include "../driver/if_80211.hpp"

namespace odtone {
namespace networkmanager {

class DeviceWireless :
	public Device,
	public org::freedesktop::NetworkManager::Device::Wireless_adaptor
{
	enum NM_802_11_MODE {
		NM_802_11_MODE_UNKNOWN = 0,
		NM_802_11_MODE_ADHOC   = 1,
		NM_802_11_MODE_INFRA   = 2
	};

	enum NM_802_11_DEVICE_CAP {
		NM_802_11_DEVICE_CAP_NONE          = 0x0,
		NM_802_11_DEVICE_CAP_CIPHER_WEP40  = 0x1,
		NM_802_11_DEVICE_CAP_CIPHER_WEP104 = 0x2,
		NM_802_11_DEVICE_CAP_CIPHER_TKIP   = 0x4,
		NM_802_11_DEVICE_CAP_CIPHER_CCMP   = 0x8,
		NM_802_11_DEVICE_CAP_WPA           = 0x10,
		NM_802_11_DEVICE_CAP_RSN           = 0x20
	};

public:
	DeviceWireless(DBus::Connection &connection, const char* path, odtone::mih::mac_addr &address);
	~DeviceWireless();

	// inherited from Device adaptor
	void Disconnect();

	// inherited from Wireless adaptor
	std::vector< ::DBus::Path > GetAccessPoints();

	// override from PropertiesAdaptor
	void on_get_property(DBus::InterfaceAdaptor &interface, const std::string &property, DBus::Variant &value);

private:
	if_80211       _fi;

	std::map<DBus::Path, AccessPoint> _access_points_map;

	std::string    _path;
	odtone::logger log_;
};

}; };

#endif /* NETWORKMANAGER_DEVICEWIRELESS__HPP_ */
