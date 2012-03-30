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

#include "AccessPoint.hpp"

#include "DeviceWireless.hpp"
#include "boost/foreach.hpp"

using namespace odtone::networkmanager;

AccessPoint::AccessPoint(DBus::Connection &connection, const char* path, poa_info i) :
	DBus::ObjectAdaptor(connection, path),
	_path(path)
{
	// strength
	mih::percentage *signal_pct = boost::get<mih::percentage>(&i.signal);
	if (signal_pct) {
		Strength = /*(uint)*/*signal_pct;
	} else {
		sint8 *signal_dbm = boost::get<sint8>(&i.signal);
		Strength = dbm_to_percent(*signal_dbm);
	}

	// bitrate
	MaxBitrate = i.data_rate;

	// mode (assumed!)
	Mode = DeviceWireless::NM_802_11_MODE_INFRA;

	// bssid
	HwAddress = "";
	mih::link_addr *mac_ = boost::get<mih::link_addr>(&i.id.poa_addr);
	if (mac_) {
		mih::mac_addr *mac = boost::get<mih::mac_addr>(mac_);
		if (mac_) {
			HwAddress = mac->address();
		}
	}

	// frequency
	Frequency = channel_to_frequency(i.channel_id);

	// ssid
	std::vector<uint8_t> vec;
	for (uint r = 0; r < i.network_id.size(); r++) {
		vec.push_back(i.network_id[r]);
	}
	Ssid = vec;

	// RSN (WPA2) flags
	RsnFlags = parse_security_flags(i.rsn);

	// WPA flags
	WpaFlags = parse_security_flags(i.wpa);

	// common flags
	Flags = parse_common_flags(i);
}

AccessPoint::~AccessPoint()
{
}

uint32_t AccessPoint::dbm_to_percent(sint dbm)
{
	// assume between -40 and -100, like NetworkManager
	if (dbm > -40) {
		dbm = -40;
	} else if (dbm < -100) {
		dbm = -100;
	}

	dbm += 40; // normalize to 0
	return 100 - ((double)-dbm * 100.0) / 60.0;
}

uint32_t AccessPoint::channel_to_frequency(uint channel)
{
	uint ret = 0;

	if (channel < 13) {           // 2.4GHz
		ret = channel * 5 + 2407;
	} else {                      // 5GHz, and possibly some errors
		ret = channel * 5 + 5000;
	}

	return ret;
}

uint32_t AccessPoint::parse_security_flags(const std::vector<nlwrap::security_features> &fts)
{
	uint32_t r = NM_802_11_AP_SEC_NONE;

	BOOST_FOREACH (const nlwrap::security_features &ft, fts) {
		switch (ft)
		{
			case nlwrap::pairwise_wep40:
				r |= NM_802_11_AP_SEC_PAIR_WEP40;
				break;
			case nlwrap::pairwise_wep104:
				r |= NM_802_11_AP_SEC_PAIR_WEP104;
				break;
			case nlwrap::pairwise_tkip:
				r |= NM_802_11_AP_SEC_PAIR_TKIP;
				break;
			case nlwrap::pairwise_ccmp:
				r |= NM_802_11_AP_SEC_PAIR_CCMP;
				break;

			case nlwrap::group_wep40:
				r |= NM_802_11_AP_SEC_GROUP_WEP40;
				break;
			case nlwrap::group_wep104:
				r |= NM_802_11_AP_SEC_GROUP_WEP104;
				break;
			case nlwrap::group_tkip:
				r |= NM_802_11_AP_SEC_GROUP_TKIP;
				break;
			case nlwrap::group_ccmp:
				r |= NM_802_11_AP_SEC_GROUP_CCMP;
				break;

			case nlwrap::mgmt_psk:
				r |= NM_802_11_AP_SEC_KEY_MGMT_PSK;
				break;
			case nlwrap::mgmt_802_1x:
				r |= NM_802_11_AP_SEC_KEY_MGMT_802_1X;
				break;
		}
	}

	return r;
}

uint32_t AccessPoint::parse_common_flags(const poa_info &i)
{
	uint32_t r = NM_802_11_AP_FLAGS_NONE;

	if (i.bss_privacy_capable) {
		if (i.bss_privacy_capable.get()) {
			r = NM_802_11_AP_FLAGS_PRIVACY;
		}
	}

	return r;
}
