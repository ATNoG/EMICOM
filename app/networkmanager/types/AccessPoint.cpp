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

AccessPoint::AccessPoint(DBus::Connection &connection, const char* path, mih::link_det_info i) :
	DBus::ObjectAdaptor(connection, path),
	_path(path),
	log_(_path.c_str(), std::cout),
	_last_change(boost::posix_time::microsec_clock::universal_time())
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
	mih::mac_addr mac = boost::get<mih::mac_addr>(boost::get<mih::link_addr>(i.id.poa_addr));
	HwAddress = mac.address();

	// frequency
	// TODO
	//Frequency = channel_to_frequency(i.channel_id);

	// ssid
	std::vector<uint8_t> vec;
	for (uint r = 0; r < i.network_id.size(); r++) {
		vec.push_back(i.network_id[r]);
	}
	Ssid = vec;

	// RSN (WPA2) flags
	//RsnFlags = parse_security_flags(i.rsn);

	// WPA flags
	//WpaFlags = parse_security_flags(i.wpa);

	// common flags
	//Flags = parse_common_flags(i);
}

AccessPoint::~AccessPoint()
{
}

void AccessPoint::Update(mih::link_det_info i)
{
	log_(0, "Updating info");

	_last_change = boost::posix_time::microsec_clock::universal_time();

//	uint32_t               _Flags;
//	uint32_t               _WpaFlags;
//	uint32_t               _RsnFlags;
	std::vector< uint8_t > _Ssid;
//	uint32_t               _Frequency;
	std::string            _HwAddress;
	//uint32_t               _Mode;
	uint32_t               _MaxBitrate;
	uint8_t                _Strength;

	std::map<std::string, DBus::Variant> props;

	// strength
	mih::percentage *signal_pct = boost::get<mih::percentage>(&i.signal);
	if (signal_pct) {
		_Strength = /*(uint)*/*signal_pct;
	} else {
		sint8 *signal_dbm = boost::get<sint8>(&i.signal);
		_Strength = dbm_to_percent(*signal_dbm);
	}

	if (_Strength != Strength()) {
		Strength = _Strength;

		DBus::Variant tmp;
		tmp.writer().append_byte(Strength());
		props["Strength"] = tmp;
	}

	// bitrate
	_MaxBitrate = i.data_rate;
	if (_MaxBitrate != MaxBitrate()) {
		MaxBitrate = _MaxBitrate;

		DBus::Variant tmp;
		tmp.writer().append_uint32(MaxBitrate());
		props["MaxBitrate"] = tmp;
	}

	// bssid
	_HwAddress = "";
	mih::mac_addr mac = boost::get<mih::mac_addr>(boost::get<mih::link_addr>(i.id.poa_addr));
	_HwAddress = mac.address();
	if (_HwAddress != HwAddress()) {
		HwAddress = _HwAddress;

		DBus::Variant tmp;
		tmp.writer().append_string(HwAddress().c_str());
		props["HwAddress"] = tmp;
	}

	// frequency
	// TODO
/*	_Frequency = channel_to_frequency(i.channel_id);
	if (_Frequency != Frequency()) {
		Frequency = _Frequency;

		DBus::Variant tmp;
		tmp.writer().append_uint32(Frequency());
		props["Frequency"] = tmp;
	}*/

	// ssid
	std::vector<uint8_t> vec;
	for (uint r = 0; r < i.network_id.size(); r++) {
		vec.push_back(i.network_id[r]);
	}
	_Ssid = vec;

	if (_Ssid != Ssid()) {
		Ssid = _Ssid;

		DBus::Variant tmp;
		DBus::MessageIter iter = tmp.writer();
		iter << Ssid();
		props["Ssid"] = tmp;
	}

	// RSN (WPA2) flags
/*	_RsnFlags = parse_security_flags(i.rsn);
	if (_RsnFlags != RsnFlags()) {
		RsnFlags = _RsnFlags;

		DBus::Variant tmp;
		tmp.writer().append_uint32(RsnFlags());
		props["RsnFlags"] = tmp;
	}*/

	// WPA flags
/*	_WpaFlags = parse_security_flags(i.wpa);
	if (_WpaFlags != WpaFlags()) {
		WpaFlags = _WpaFlags;

		DBus::Variant tmp;
		tmp.writer().append_uint32(WpaFlags());
		props["WpaFlags"] = tmp;
	}*/

	// common flags
/*	_Flags = parse_common_flags(i);
	if (_Flags != Flags()) {
		Flags = _Flags;

		DBus::Variant tmp;
		tmp.writer().append_uint32(Flags());
		props["Flags"] = tmp;
	}*/

	if (!props.empty()) {
		PropertiesChanged(props);
	}
}

boost::posix_time::time_duration AccessPoint::last_change()
{
	boost::posix_time::ptime now(boost::posix_time::microsec_clock::universal_time());

	return boost::posix_time::time_period(_last_change, now).length();
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
