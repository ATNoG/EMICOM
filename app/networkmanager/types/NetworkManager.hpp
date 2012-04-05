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
#include "../driver/if_80211.hpp"

namespace odtone {
namespace networkmanager {

class NetworkManager : boost::noncopyable,
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
	 * Name for D-Bus NetworkManager interface.
	 */
	static const char* const NAME;

	/**
	 * Path for D-Bus NetworkManager object.
	 */
	static const char* const PATH;

	/**
	 * Path for Settings saving.
	 */
	static const char* const SETTINGS_DIR;

	/**
	 * Construct a new NetworkManager D-Bus interface data type.
	 *
	 * @param connection The D-Bus connection to create on.
	 */
	NetworkManager(DBus::Connection &connection);

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
	 * Add and activate a connection, automatically filling properties with a given device and specific object.
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
	                              ::DBus::Path& path, ::DBus::Path& active_connection);

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

	/**
	 * Add a new 802.11 device, to be looked in the system by MAC Address.
	 *
	 * @param address The MAC Address of the device.
	 */
	void add_802_11_device(odtone::mih::mac_addr &address);

private:
	void state(NM_STATE newstate);

private:
	DBus::Connection &_connection;
	Settings          _settings;
	odtone::logger     log_;

	std::map<DBus::Path, std::unique_ptr<Device>> _device_map;
};

}; };

#endif /* NETWORKMANAGER_NETWORKMANAGER__HPP_ */
