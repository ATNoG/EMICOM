//=============================================================================
// Brief   : GENetlink message RAI wrapper
// Authors : André Prata <andreprata@av.it.pt>
//-----------------------------------------------------------------------------
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

#include "genl_msg.hpp"

#include <netlink/genl/genl.h>
#include <linux/nl80211.h>

#include <iostream>

namespace nlwrap {

#define ETH_ALEN 18

#define IE_ARRAY_SIZE 128
#define IE_WIFI_SIZE 4

// for ESSID
#define IE_SSID_INDEX 0

// for data rates
#define IE_SUPP_RATES_INDEX 1
#define IE_EXTENDED_SUPP_RATES_INDEX 50

// for security features
#define IE_RNS_INDEX 48
#define IE_VENDOR 221 // doesn't count for ARRAY_SIZE

#define WLAN_CAPABILITY_QOS (1<<9)

struct bss_parse_policy {
	::nla_policy pol[NL80211_BSS_MAX + 1];
	bss_parse_policy();
} bss_policy;

void mac_addr_n2a(char *mac_addr, unsigned char *arg);

////////////////////////////////////////////////////////////////////////

genl_msg::genl_msg()
{
	_msg = ::nlmsg_alloc();
	if (!_msg) {
		throw "Error allocating netlink message";
	}
	_own = true;
}

genl_msg::genl_msg(int family, int type, int flags)
{
	_msg = ::nlmsg_alloc();
	if (!_msg) {
		throw "Error allocating netlink message";
	}
	_own = true;

	::genlmsg_put(_msg, 0, 0, family, 0, flags, type, 0);
}

genl_msg::genl_msg(::nl_msg *msg)
{
	_own = false;

	::nlmsghdr    *nlh = ::nlmsg_hdr(msg);
	::genlmsghdr *gnlh = static_cast< ::genlmsghdr * >(::nlmsg_data(nlh));
	_cmd = gnlh->cmd;

	// parse attr
	::nlattr *tb[NL80211_ATTR_MAX + 1];
	if (nla_parse(tb, NL80211_ATTR_MAX, ::genlmsg_attrdata(gnlh, 0), ::genlmsg_attrlen(gnlh, 0), NULL)) {
		throw "Error parsing nl_msg attributes";
	}
	parse_attr(tb);

	// parse bss
	if (!tb[NL80211_ATTR_BSS]) {
		return;
	}

	::nlattr *bss[NL80211_BSS_MAX + 1];
	if (::nla_parse_nested(bss, NL80211_BSS_MAX, tb[NL80211_ATTR_BSS], bss_policy.pol)) {
		throw "Error parsing nl_msg bss attributes";
	}
	parse_bss(bss);

	// parse information elements
	if (!bss[NL80211_BSS_INFORMATION_ELEMENTS]) {
		return;
	}

	unsigned char *ie = static_cast<unsigned char *>(::nla_data(bss[NL80211_BSS_INFORMATION_ELEMENTS]));
	int ielen = ::nla_len(bss[NL80211_BSS_INFORMATION_ELEMENTS]);
	parse_information_elements(ie, ielen);
}

genl_msg::~genl_msg()
{
	if (_own && _msg) {
		::nlmsg_free(_msg);
	}
}

genl_msg::operator ::nl_msg *()
{
	return _msg;
}

void genl_msg::put_ifindex(int ifindex)
{
	if (::nla_put_u32(_msg, NL80211_ATTR_IFINDEX, ifindex)) {
		throw "Error putting ifindex in nl_msg";
	}
}

void genl_msg::put_family_name(std::string name)
{
	if (::nla_put_string(_msg, CTRL_ATTR_FAMILY_NAME, name.c_str())) {
		throw "Error putting family_name in nl_msg";
	}
}

void genl_msg::put_ps_state(int state)
{
	if (::nla_put_u32(_msg, NL80211_ATTR_PS_STATE, state)) {
		throw "Error putting PS_STATE in nl_msg";
	}
}


int genl_msg::cmd()
{
	return _cmd;
}

void genl_msg::parse_attr(::nlattr *tb[NL80211_ATTR_MAX + 1])
{
	if (tb[NL80211_ATTR_IFINDEX]) {
		attr_ifindex = ::nla_get_u32(tb[NL80211_ATTR_IFINDEX]);
	}

	if (tb[NL80211_ATTR_STATUS_CODE]) {
		attr_status_code = ::nla_get_u16(tb[NL80211_ATTR_STATUS_CODE]);
	}

	if (tb[NL80211_ATTR_MAC]) {
		char addr[ETH_ALEN];
		mac_addr_n2a(addr, static_cast<unsigned char *>(::nla_data(tb[NL80211_ATTR_MAC])));
		attr_mac = std::string(addr, ETH_ALEN);
	}

	if (tb[NL80211_ATTR_REASON_CODE]) {
		attr_reason_code = ::nla_get_u16(tb[NL80211_ATTR_REASON_CODE]);
	}

	if (tb[NL80211_ATTR_PS_STATE]) {
		attr_ps_state = ::nla_get_u32(tb[NL80211_ATTR_PS_STATE]);
	}

	if (tb[NL80211_ATTR_BSS]) {
		attr_bss = true;
	}
}

void genl_msg::parse_bss(::nlattr *bss[NL80211_BSS_MAX + 1])
{
	if (bss[NL80211_BSS_STATUS]) {
		bss_status = ::nla_get_u32(bss[NL80211_BSS_STATUS]);
	}

	if (bss[NL80211_BSS_FREQUENCY]) {
		bss_frequency = ::nla_get_u32(bss[NL80211_BSS_FREQUENCY]);
	}

	if (bss[NL80211_BSS_BSSID]) {
		char addr[ETH_ALEN];
		mac_addr_n2a(addr, static_cast<unsigned char *>(::nla_data(bss[NL80211_BSS_BSSID])));
		bss_bssid = std::string(addr, ETH_ALEN);
	}

	if (bss[NL80211_BSS_SIGNAL_MBM]) {
		bss_signal_mbm = static_cast<int>(::nla_get_u32(bss[NL80211_BSS_SIGNAL_MBM])) / 100;
	}

	if (bss[NL80211_BSS_CAPABILITY]) {
		bss_qos_capable = ::nla_get_u16(bss[NL80211_BSS_CAPABILITY]) & WLAN_CAPABILITY_QOS;
	}
}

void genl_msg::parse_information_elements(unsigned char *ie, int ielen)
{
	static unsigned char ms_oui[3] = { 0x00, 0x50, 0xf2 };

	unsigned int max_rate = 0;
	unsigned int rate;

	unsigned char* data;

	while (ielen >= 2 && ielen >= ie[1]) {
		if (ie[0] >= IE_ARRAY_SIZE) {
			break;
		}
		data = ie + 2;

		switch (ie[0]) {
		case IE_SSID_INDEX:
		    ie_ssid = std::string(reinterpret_cast<const char *>(data), ie[1]);
		    break;

		case IE_SUPP_RATES_INDEX:
		case IE_EXTENDED_SUPP_RATES_INDEX:
			for (unsigned int r = 0; r < ie[1]; r++) {
				rate = (data[r] & 0x7f) / 2;
				if (rate > max_rate) {
					max_rate = rate;
					ie_max_data_rate = max_rate;
				}
			}
			break;

		case IE_RNS_INDEX:
			ie_has_security_features = true;
			break;

		case IE_VENDOR:
			if (ie[1] >= 4 && ::memcmp(data, ms_oui, 3) == 0) {
				ie_has_security_features = true;
			}
			break;

		default:
			break;
		}

		ielen -= ie[1] + 2;
		ie += ie[1] + 2;
	}
}

////////////////////////////////////////////////////////////////////////

void mac_addr_n2a(char *mac_addr, unsigned char *arg)
{
	int i, l;

	l = 0;
	for (i = 0; i < 6; i++) {
		if (i == 0) {
			sprintf(mac_addr+l, "%02x", arg[i]);
			l += 2;
		} else {
			sprintf(mac_addr+l, ":%02x", arg[i]);
			l += 3;
		}
	}
}

bss_parse_policy::bss_parse_policy() {
	pol[NL80211_BSS_TSF].type = NLA_U64;
	pol[NL80211_BSS_FREQUENCY].type = NLA_U32;
	pol[NL80211_BSS_BSSID] = { };
	pol[NL80211_BSS_BEACON_INTERVAL].type = NLA_U16;
	pol[NL80211_BSS_CAPABILITY].type = NLA_U16;
	pol[NL80211_BSS_INFORMATION_ELEMENTS] = { };
	pol[NL80211_BSS_SIGNAL_MBM].type = NLA_U32;
	pol[NL80211_BSS_SIGNAL_UNSPEC].type = NLA_U8;
	pol[NL80211_BSS_STATUS].type = NLA_U32;
	pol[NL80211_BSS_SEEN_MS_AGO].type = NLA_U32;
	pol[NL80211_BSS_BEACON_IES] = { };
}

}