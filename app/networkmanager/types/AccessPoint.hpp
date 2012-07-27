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

#include <boost/date_time/posix_time/posix_time.hpp>
#include "../dbus/adaptors/AccessPoint.hpp"
#include <boost/noncopyable.hpp>
#include "odtone/logger.hpp"

#include <odtone/mih/tlv_types.hpp>

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
	struct bss_id {
		std::string ssid;
		//odtone::uint32 freq;
		odtone::mih::mac_addr addr;
	};

	/**
	 * Construct a new AccessPoint D-Bus interface data type.
	 *
	 * @param connection The D-Bus (system) connection.
	 * @param path The D-Bus path for this object.
	 * @param i The associated underlying point of access information.
	 */
	AccessPoint(DBus::Connection &connection, const char* path, mih::link_det_info i);

	/**
	 * Destroy this object.
	 */
	~AccessPoint();

	/**
	 * Update this AP with new POA information
	 */
	void Update(mih::link_det_info i);

	/**
	 * Update just the RSSI from this AP.
	 */
	void update_strength(mih::sig_strength &s);

	/**
	 * Get the elapsed time since the last change on this object.
	 *
	 * @return The time that elapsed since the creation or last change.
	 */
	boost::posix_time::time_duration last_change();

	/**
	 * Get the ap network id tuple.
	 *
	 * @return the set of elements that identify a bss.
	 */
	bss_id get_id();

	/**
	 * Convert a dBm signal value to a percentage
	 *
	 * @param dbm The current signal strength, in dBm.
	 * @return The assumed percentage for the quality signal.
	 */
	static uint32_t dbm_to_percent(sint dbm);

	/**
	 * Convert a channel id value to frequency.
	 *
	 * @param channel The channel id.
	 * @return The associated radio frequency.
	 */
	static uint32_t channel_to_frequency(uint channel);

//private:
//	uint32_t parse_security_flags(const std::vector<nlwrap::security_features> &fts);
//	uint32_t parse_common_flags(const poa_info &i);
//
private:
	std::string _path;
	logger      log_;

	boost::posix_time::ptime _last_change;
	bss_id _id;
};

}; };

#endif /* NETWORKMANAGER_ACCESSPOINT__HPP_ */
