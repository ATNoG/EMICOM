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

#ifndef __NLWRAP_GENL_MSG_
#define __NLWRAP_GENL_MSG_

#include <boost/noncopyable.hpp>
#include <boost/optional.hpp>
#include <string>

#include <netlink/msg.h>
#include <linux/nl80211.h>

namespace nlwrap {

enum security_features {
	pairwise_wep40,
	pairwise_wep104,
	pairwise_tkip,
	pairwise_ccmp,

	group_wep40,
	group_wep104,
	group_tkip,
	group_ccmp,

	mgmt_psk,
	mgmt_802_1x
};

/**
 * This class provides a RAI wrapper for the nl_msg datatype
 */
class genl_msg : boost::noncopyable {

public:
	/**
	 * Construct a new genl_msg object. Allocates a new nl_msg counterpart.
	 */
	genl_msg();

	/**
	 * Construct a new genl_msg object, providing a family, command and flags.
	 *
	 * @param family The family of the message recipient.
	 * @param type The command of the message.
	 * @param flags Aditional message flags.
	 */
	genl_msg(int family, int type, int flags);

	/**
	 * Construct a message from an already allocate nl_msg object
	 * and automatically attempt parsing TLV elements.
	 * 
	 * @warning This does not deallocate the object upon destruction.
	 *
	 * @param nl_msg The preallocated nl_msg object.
	 */
	genl_msg(::nl_msg *msg);

	/**
	 * Destruct the genl_msg object.
	 * Frees the nl_msg counterpart, if allocated in the object's context.
	 */
	~genl_msg();

	/**
	 * Allow direct usage of the underlying nl_msg pointer.
	 */
	operator ::nl_msg *();

	/**
	 * Put an IFINDEX element in the message.
	 *
	 * @param ifindex The desired IFINDEX.
	 */
	void put_ifindex(int ifindex);

	/**
	 * Put a FAMILY_NAME string into the message.
	 *
	 * @param name The desired FAMILY_NAME.
	 */
	void put_family_name(std::string name);

	/**
	 * Put a PS_STATE value in the message.
	 *
	 * @param state The desired PS_STATE.
	 */
	void put_ps_state(int state);

	/**
	 * Put a MAC attribute in the message.
	 *
	 * @param mac The desired MAC attribute.
	 */
	void put_mac_address(std::string mac);

	/**
	 * Get the command from a parsed message.
	 *
	 * @return The command from the parsed message.
	 */
	int cmd();

	boost::optional<int> attr_ifindex;
	boost::optional<int> attr_status_code;
	boost::optional<std::string> attr_mac;
	boost::optional<int> attr_reason_code;
	boost::optional<int> attr_ps_state;
	boost::optional<bool> attr_bss;

	boost::optional<int> bss_status;
	boost::optional<int> bss_frequency;
	boost::optional<std::string> bss_bssid;
	boost::optional<int> bss_signal_mbm;
	boost::optional<bool> bss_qos_capable;

	boost::optional<std::string> ie_ssid;
	boost::optional<unsigned int> ie_max_data_rate;
	boost::optional<bool> ie_has_security_features;

	boost::optional<bool> bss_privacy_capable;

	boost::optional<std::vector<security_features>> wpa;
	boost::optional<std::vector<security_features>> rsn;
	
	/**
	 * Current bitrate, in kilobits/second
	 */
	boost::optional<int> bitrate;

	boost::optional<int> iftype;

private:
	void parse_attr(::nlattr *tb[NL80211_ATTR_MAX + 1]);
	void parse_bss(::nlattr *bss[NL80211_BSS_MAX + 1]);
	void parse_information_elements(const uint8_t *ie, int ielen);
	void parse_rate(::nlattr *rate[NL80211_RATE_INFO_MAX + 1]);

	std::vector<security_features> parse_security_features(
		const uint8_t *data,
		unsigned int len,
		security_features default_pair_cipher,
		security_features default_group_cipher);

private:
	bool      _own;
	::nl_msg *_msg;

	int       _cmd;

	static unsigned char ms_oui[3];
	static unsigned char ieee80211_oui[3];
};

}

// EOF ////////////////////////////////////////////////////////////////////////

#endif /* __NLWRAP_GENL_MSG_ */
