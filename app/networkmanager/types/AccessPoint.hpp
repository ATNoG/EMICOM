//==============================================================================
// Brief   : NetworkManager AccessPoint interface implementation
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

#ifndef NETWORKMANAGER_ACCESSPOINT__HPP_
#define NETWORKMANAGER_ACCESSPOINT__HPP_

#include "../dbus/adaptors/AccessPoint.hpp"
#include <boost/noncopyable.hpp>

#include "../driver/if_80211.hpp"

namespace odtone {
namespace networkmanager {

class AccessPoint : boost::noncopyable,
	public org::freedesktop::NetworkManager::AccessPoint_adaptor,
	public DBus::IntrospectableAdaptor,
	public DBus::PropertiesAdaptor,
	public DBus::ObjectAdaptor
{
	enum NM_802_11_AP_FLAGS {
		NM_802_11_AP_FLAGS_NONE    = 0x0,
		NM_802_11_AP_FLAGS_PRIVACY = 0x1
	};

	enum NM_802_11_AP_SEC {
		NM_802_11_AP_SEC_NONE            = 0x0,

		NM_802_11_AP_SEC_PAIR_WEP40      = 0x1,
		NM_802_11_AP_SEC_PAIR_WEP104     = 0x2,
		NM_802_11_AP_SEC_PAIR_TKIP       = 0x4,
		NM_802_11_AP_SEC_PAIR_CCMP       = 0x8,

		NM_802_11_AP_SEC_GROUP_WEP40     = 0x10,
		NM_802_11_AP_SEC_GROUP_WEP104    = 0x20,
		NM_802_11_AP_SEC_GROUP_TKIP      = 0x40,
		NM_802_11_AP_SEC_GROUP_CCMP      = 0x80,

		NM_802_11_AP_SEC_KEY_MGMT_PSK    = 0x100,
		NM_802_11_AP_SEC_KEY_MGMT_802_1X = 0x200
	};

public:
	AccessPoint(DBus::Connection &connection, const char* path, poa_info i);
	~AccessPoint();

	static uint32_t dbm_to_percent(sint dbm);
	static uint32_t channel_to_frequency(uint channel);

private:
	uint32_t parse_security_flags(const std::vector<nlwrap::security_features> &fts);
	uint32_t parse_common_flags(const poa_info &i);

private:
	std::string _path;
};

}; };

#endif /* NETWORKMANAGER_ACCESSPOINT__HPP_ */
