//==============================================================================
// Brief   : NetworkManager Device specific types
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

#ifndef NETWORKMANAGER_DEVICE__HPP_
#define NETWORKMANAGER_DEVICE__HPP_

#include "../dbus/adaptors/Device.hpp"
#include <boost/noncopyable.hpp>

#include "odtone/mih/types/address.hpp"
#include "../mih_user.hpp"

namespace odtone {
namespace networkmanager {

class Device : boost::noncopyable,
	public org::freedesktop::NetworkManager::Device_adaptor,
	public DBus::IntrospectableAdaptor,
	public DBus::PropertiesAdaptor,
	public DBus::ObjectAdaptor
{
public:
	typedef boost::function<void(bool success)> completion_handler;

public:

	/**
	 * Enumeration of possible Device Types.
	 */
	enum NM_DEVICE_TYPE {
		NM_DEVICE_TYPE_UNKNOWN    = 0,
		NM_DEVICE_TYPE_ETHERNET   = 1,
		NM_DEVICE_TYPE_WIFI       = 2,
		NM_DEVICE_TYPE_UNUSED1    = 3,
		NM_DEVICE_TYPE_UNUSED2    = 4,
		NM_DEVICE_TYPE_BT         = 5,
		NM_DEVICE_TYPE_OLPC_MESH  = 6,
		NM_DEVICE_TYPE_WIMAX      = 7,
		NM_DEVICE_TYPE_MODEM      = 8,
		NM_DEVICE_TYPE_INFINIBAND = 9,
		NM_DEVICE_TYPE_BOND       = 10,
		NM_DEVICE_TYPE_VLAN       = 11
	};

	/**
	 * Enumeration of possible Device States.
	 */
	enum NM_DEVICE_STATE {
		NM_DEVICE_STATE_UNKNOWN      = 0,
		NM_DEVICE_STATE_UNMANAGED    = 10,
		NM_DEVICE_STATE_UNAVAILABLE  = 20,
		NM_DEVICE_STATE_DISCONNECTED = 30,
		NM_DEVICE_STATE_PREPARE      = 40,
		NM_DEVICE_STATE_CONFIG       = 50,
		NM_DEVICE_STATE_NEED_AUTH    = 60,
		NM_DEVICE_STATE_IP_CONFIG    = 70,
		NM_DEVICE_STATE_IP_CHECK     = 80,
		NM_DEVICE_STATE_SECONDARIES  = 90,
		NM_DEVICE_STATE_ACTIVATED    = 100,
		NM_DEVICE_STATE_DEACTIVATING = 110,
		NM_DEVICE_STATE_FAILED       = 120
	};

	/**
	 * Enumeration of possible Device State Change reasons.
	 */
	enum NM_DEVICE_STATE_REASON {
		NM_DEVICE_STATE_REASON_UNKNOWN            = 0,
		NM_DEVICE_STATE_REASON_NONE               = 1,
		NM_DEVICE_STATE_REASON_NOW_MANAGED        = 2,
		NM_DEVICE_STATE_REASON_NOW_UNMANAGED      = 3,
		NM_DEVICE_STATE_REASON_CONFIG_FAILED      = 4,
		NM_DEVICE_STATE_REASON_CONFIG_UNAVAILABLE = 5,
		NM_DEVICE_STATE_REASON_CONFIG_EXPIRED     = 6,
		NM_DEVICE_STATE_REASON_NO_SECRETS         = 7,

		NM_DEVICE_STATE_REASON_SUPPLICANT_DISCONNECT    = 8,
		NM_DEVICE_STATE_REASON_SUPPLICANT_CONFIG_FAILED = 9,
		NM_DEVICE_STATE_REASON_SUPPLICANT_FAILED        = 10,
		NM_DEVICE_STATE_REASON_SUPPLICANT_TIMEOUT       = 11,

		NM_DEVICE_STATE_REASON_PPP_START_FAILED = 12,
		NM_DEVICE_STATE_REASON_PPP_DISCONNECT   = 13,
		NM_DEVICE_STATE_REASON_PPP_FAILED       = 14,

		NM_DEVICE_STATE_REASON_DHCP_START_FAILED = 15,
		NM_DEVICE_STATE_REASON_DHCP_ERROR        = 16,
		NM_DEVICE_STATE_REASON_DHCP_FAILED       = 17,

		NM_DEVICE_STATE_REASON_SHARED_START_FAILED = 18,
		NM_DEVICE_STATE_REASON_SHARED_FAILED       = 19,

		NM_DEVICE_STATE_REASON_AUTOIP_START_FAILED = 20,
		NM_DEVICE_STATE_REASON_AUTOIP_ERROR        = 21,
		NM_DEVICE_STATE_REASON_AUTOIP_FAILED       = 22,

		NM_DEVICE_STATE_REASON_MODEM_BUSY         = 23,
		NM_DEVICE_STATE_REASON_MODEM_NO_DIAL_TONE = 24,
		NM_DEVICE_STATE_REASON_MODEM_NO_CARRIER   = 25,
		NM_DEVICE_STATE_REASON_MODEM_DIAL_TIMEOUT = 26,
		NM_DEVICE_STATE_REASON_MODEM_DIAL_FAILED  = 27,
		NM_DEVICE_STATE_REASON_MODEM_INIT_FAILED  = 28,

		NM_DEVICE_STATE_REASON_GSM_APN_FAILED                 = 29,
		NM_DEVICE_STATE_REASON_GSM_REGISTRATION_NOT_SEARCHING = 30,
		NM_DEVICE_STATE_REASON_GSM_REGISTRATION_DENIED        = 31,
		NM_DEVICE_STATE_REASON_GSM_REGISTRATION_TIMEOUT       = 32,
		NM_DEVICE_STATE_REASON_GSM_REGISTRATION_FAILED        = 33,
		NM_DEVICE_STATE_REASON_GSM_PIN_CHECK_FAILED           = 34,

		NM_DEVICE_STATE_REASON_FIRMWARE_MISSING     = 35,
		NM_DEVICE_STATE_REASON_REMOVED              = 36,
		NM_DEVICE_STATE_REASON_SLEEPING             = 37,
		NM_DEVICE_STATE_REASON_CONNECTION_REMOVED   = 38,
		NM_DEVICE_STATE_REASON_USER_REQUESTED       = 39,
		NM_DEVICE_STATE_REASON_CARRIER              = 40,
		NM_DEVICE_STATE_REASON_CONNECTION_ASSUMED   = 41,
		NM_DEVICE_STATE_REASON_SUPPLICANT_AVAILABLE = 42,
		NM_DEVICE_STATE_REASON_MODEM_NOT_FOUND      = 43,
		NM_DEVICE_STATE_REASON_BT_FAILED            = 44,

		NM_DEVICE_STATE_REASON_GSM_SIM_NOT_INSERTED = 45,
		NM_DEVICE_STATE_REASON_GSM_SIM_PIN_REQUIRED = 46,
		NM_DEVICE_STATE_REASON_GSM_SIM_PUK_REQUIRED = 47,
		NM_DEVICE_STATE_REASON_GSM_SIM_WRONG        = 48,
		NM_DEVICE_STATE_REASON_INFINIBAND_MODE      = 49,
		NM_DEVICE_STATE_REASON_DEPENDENCY_FAILED    = 50
	};

	/**
	 * Enumeration of possible Device Capabilities.
	 */
	enum NM_DEVICE_CAP {
		NM_DEVICE_CAP_NONE           = 0x0,
		NM_DEVICE_CAP_NM_SUPPORTED   = 0x1,
		NM_DEVICE_CAP_CARRIER_DETECT = 0x2
	};

	/**
	 * Construct a new Device D-Bus interface data type.
	 *
	 * @param connection   The D-Bus (system) connection to use.
	 * @param path         This object's D-Bus path.
	 * @param control_user Reference to an mih_user for interface controlling.
	 * @param lti          A tuple that identifies this interface.
	 */
	Device(DBus::Connection &connection, const char* path,
	       odtone::networkmanager::mih_user &control_user, mih::link_tuple_id &lti);

	/**
	 * Destroy this object.
	 */
	~Device();

	/**
	 * Call a Disconnect on this device.
	 */
	virtual void Disconnect();

	/**
	 * Enable this device. Don't necessarily connect, just bring up!
	 */
	virtual void Enable();

	/**
	 * Disable this device. Meaning cut the power.
	 */
	virtual void Disable();

	/**
	 * Inform this device that L2 connectivity was dropped.
	 */
	virtual void link_down() = 0;

	/**
	 * Inform this device that L2 connectivity is up.
	 */
	virtual void link_up(const boost::optional<mih::mac_addr> &poa) = 0;

	/**
	 * Send a link_conf command to this device.
	 */
	virtual void link_conf(const completion_handler &h,
	                       const boost::optional<mih::network_id> &network,
	                       const mih::configuration_list &lconf);

	/**
	 * Send an l3_conf command to this device.
	 */
	virtual void l3_conf(const completion_handler &h,
	                     const mih::ip_cfg_methods &cfg_methods,
	                     const boost::optional<mih::ip_info_list> &address_list,
	                     const boost::optional<mih::ip_info_list> &route_list,
	                     const boost::optional<mih::ip_addr_list> &dns_list,
	                     const boost::optional<mih::fqdn_list> &domain_list);

protected:

	/**
	 * Auxiliary function to change and notify (signal) state changes on this object.
	 */
	void state(NM_DEVICE_STATE newstate, NM_DEVICE_STATE_REASON reason);

protected:
	mih_user           &_ctrl;
	mih::link_tuple_id  _lti;

	std::string         _dbus_path;
	logger               log_;
};

}; };

// unused
/*#include "../adaptors/Device.hpp"

class Device :
	public org::freedesktop::NetworkManager::Device_adaptor,
	public DBus::IntrospectableAdaptor,
	public DBus::PropertiesAdaptor,
	public DBus::ObjectAdaptor
{
public:
	Device(DBus::Connection &connection, const char* path);
	~Device();

	// inherited from Device adaptor
	void Disconnect();
};*/

#endif /* NETWORKMANAGER_DEVICE__HPP_ */
