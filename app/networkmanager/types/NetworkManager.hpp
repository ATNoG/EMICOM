//==============================================================================
// Brief   : NetworkManager D-Bus interface implementation
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

#ifndef NETWORKMANAGER_NETWORKMANAGER__HPP_
#define NETWORKMANAGER_NETWORKMANAGER__HPP_

#include "../dbus/adaptors/NetworkManager.hpp"
#include <boost/noncopyable.hpp>
#include "odtone/logger.hpp"

#include "types.hpp"

#include "../mih_user.hpp"

namespace odtone {
namespace networkmanager {

/**
 * Configuration parameters
 */
static const char* const DBUS_NAME     = "org.freedesktop.NetworkManager";
static const char* const DBUS_PATH     = "/org/freedesktop/NetworkManager";

static const char* const kConf_Settings_Path = "nm.settings_path";
static const char* const kConf_Version       = "nm.version";

static const char* const kConf_Networking_Enabled = "nm.networking_enabled";
static const char* const kConf_Wireless_Enabled   = "nm.wireless_enabled";
static const char* const kConf_Wimax_Enabled      = "nm.wimax_enabled";
static const char* const kConf_Wwan_Enabled       = "nm.wwan_enabled";

class NetworkManager : boost::noncopyable,
// NetworkManager D-Bus
	public org::freedesktop::NetworkManager_adaptor,
	public DBus::IntrospectableAdaptor,
	public DBus::PropertiesAdaptor,
	public DBus::ObjectAdaptor
{
public:

	/**
	 * Enumeration of possible NetworkManager states.
	 */
	enum NM_STATE {
		NM_STATE_UNKNOWN          = 0,
		NM_STATE_ASLEEP           = 10,
		NM_STATE_DISCONNECTED     = 20,
		NM_STATE_DISCONNECTING    = 30,
		NM_STATE_CONNECTING       = 40,
		NM_STATE_CONNECTED_LOCAL  = 50,
		NM_STATE_CONNECTED_SITE   = 60,
		NM_STATE_CONNECTED_GLOBAL = 70
	};

public:
	/**
	 * Construct a new NetworkManager D-Bus interface data type.
	 *
	 * @param connection The D-Bus connection to create on.
	 * @param cfg The NetworkManager configuration parameters
	 * @param io @see mih_user::mih_user()
	 */
	NetworkManager(DBus::Connection &connection, const mih::config &cfg, boost::asio::io_service &io);

	/**
	 * Destroy this object.
	 */
	~NetworkManager();

	/**
	 * Get the state of this NetworkManager object.
	 *
	 * @return The current state.
	 *
	 * @see NetworkManager::NM_STATE.
	 */
	uint32_t state();

	/**
	 * Set the logging method of this NetworkManager instance.
	 *
	 * @param level The log level.
	 * @param domains The log operations.
	 */
	void SetLogging(const std::string& level, const std::string& domains);

	/**
	 * Get the available permissions for authenticated operations.
	 *
	 * @return A map of permissions.
	 */
	std::map< std::string, std::string > GetPermissions();

	/**
	 * Enable the NetworkManager overall operation.
	 */
	void Enable(const bool& enable);

	/**
	 * Sleep or Awake the NetworkManager.
	 *
	 * When asleep, all devices are deactivated. When awake, devices are available to activate.
	 *
	 * @param sleep If true, sleep, else awake.
	 */
	void Sleep(const bool& sleep);

	/**
	 * Deactivate a given active connection.
	 *
	 * @param active_connection The active connection to deactivate.
	 */
	void DeactivateConnection(const ::DBus::Path& active_connection);

	/**
	 * Add and activate a connection.
	 *
	 * There's no support for automatically filling properties
	 * with a given device and specific object.
	 *
	 * @param connection The connection parameters.
	 * @param device The device for automatic parameter filling.
	 * @param specific_object The specific object.
	 *
	 * @param path return: The D-Bus path of the created connection.
	 * @param asctive_connection return: The D-Bus path of the newly created connection.
	 */
	void AddAndActivateConnection(const std::map< std::string, std::map< std::string, ::DBus::Variant > >& connection,
	                              const ::DBus::Path& device,
	                              const ::DBus::Path& specific_object,
	                              ::DBus::Path& path,
	                              ::DBus::Path& active_connection);

	/**
	 * Activate a given connection.
	 *
	 * @param connection The connection to activate.
	 * @param device The device for the connection.
	 * @param specific_object The path of a connection-type-specific object this activation should use.
	 */
	::DBus::Path ActivateConnection(const ::DBus::Path& connection,
	                                const ::DBus::Path& device,
	                                const ::DBus::Path& specific_object);

	/**
	 * Get a device given its IpInterface.
	 *
	 * @param iface The IpInterface of the device.
	 *
	 * @return The object path.
	 */
	::DBus::Path GetDeviceByIpIface(const std::string& iface);

	/**
	 * Get the list of managed devices.
	 *
	 * @return The list of Device objects.
	 */
	std::vector< ::DBus::Path > GetDevices();

private:
	/**
	 * Add a new 802.11 device, to be searched in the system by MAC Address.
	 *
	 * @param address The MAC Address of the device.
	 */
	void add_802_11_device(mih::mac_addr &address);

	/**
	 * Add a new ethernet device, to be searched in the system by MAC Address.
	 *
	 * @param address The MAC Address of the device.
	 */
	void add_ethernet_device(mih::mac_addr &address);

	/**
	 * Method to inform the NetworkManager that new AccessPoints were detected.
	 */
	void new_accesspoints_detected();

	/**
	 * Method to inform the NetworkManager that a new L2 connection was completed.
	 *
	 * @param dev The device's mac address.
	 * @param poa The newly associated point of access' address.
	 */
	void link_up(const mih::mac_addr &dev, const boost::optional<mih::mac_addr> &poa);

	/**
	 * Method to inform NetworkManager that an L2 connection was dropped.
	 *
	 * @param dev The device's mac address.
	 */
	void link_down(const mih::mac_addr &dev);

	/**
	 * Method to inform the respective devices that links were detected.
	 *
	 * @param ldil The list of detected links.
	 */
	void links_detected(const std::vector<mih::link_det_info> &ldil);

	/**
	 * Method to act on various link parameter reports.
	 *
	 * @param lti      The link to which the parameters refer.
	 * @param rpt_list The report list.
	 */
	void parameters_report(const mih::link_tuple_id &lti, const mih::link_param_rpt_list &rpt_list);

	/**
	 * Default MIH event handler.
	 *
	 * @param msg Received message.
	 * @param ec Error code.
	 */
	void event_handler(mih::message &msg, const boost::system::error_code &ec);

	/**
	 * MIH handler for capability discovery, to inform of new devices.
	 *
	 */
	void new_device(mih::network_type &type, mih::link_addr &address);

protected:
	// override from PropertiesAdaptor
	/**
	 * @see DBus::PropertyAdaptor
	 */
	void on_set_property(DBus::InterfaceAdaptor &interface, const std::string &property, const DBus::Variant &value);

private:
	/**
	 * Change the NetworkManager state and notify with the associated signal.
	 *
	 * @param newstate The new state.
	 */
	void state(NM_STATE newstate);

	/**
	 * Change a NetworkManager property and notify with the associated signal.
	 *
	 * @param property The property that is to be changed.
	 * @param value    The new value of the referred property.
	 */
	template <class T>
	void property(const std::string &property, const T &value);

	/**
	 * Calculate the active connection list from the respective map
	 */
	std::vector<DBus::Path> active_connections();

	/**
	 * Proceed with the configuration of a link.
	 */
	void link_conf(const DBus::Path &device, const DBus::Path &connection_active);

	/**
	 * Configure IPs, routes and DNS in a device.
	 */
	void l3_conf(const DBus::Path &device, const DBus::Path &connection_active);

	/**
	 * Perform the necessary cleanups due to a failed
	 * or dropped connection attempt on a device.
	 */
	void clear_connections(const DBus::Path &device);

private:
	boost::asio::io_service &_io;
	DBus::Connection &_connection;
	std::string       _settings_path;

	Settings          _settings;
	AgentManager      _agent_manager; // dummy
	odtone::logger     log_;

	mih_user          _mih_user;

	std::map<DBus::Path, std::shared_ptr<Device>>           _device_map;
	std::map<DBus::Path, std::shared_ptr<ConnectionActive>> _active_connections;
	std::map<DBus::Path, DBus::Path>                        _device_active_connection; // Device,ConnectionActive
};

}; };

#endif /* NETWORKMANAGER_NETWORKMANAGER__HPP_ */
