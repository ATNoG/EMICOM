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

#include "NetworkManager.hpp"
#include "util.hpp"

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/assign/list_of.hpp>

using namespace odtone::networkmanager;
using namespace boost::assign;

NetworkManager::NetworkManager(DBus::Connection &connection, const mih::config &cfg, boost::asio::io_service &io) :
	DBus::ObjectAdaptor(connection, cfg.get<std::string>(kConf_DBus_Path).c_str()),
	_io(io),
	_connection(connection),
	_dbus_path(cfg.get<std::string>(kConf_DBus_Path)),
	_settings_path(cfg.get<std::string>(kConf_Settings_Path)),
	_settings(_connection, std::string(_dbus_path).append("/Settings").c_str(), _settings_path.c_str()),
	_agent_manager(_connection, std::string(_dbus_path).append("/AgentManager").c_str()),
	log_(_dbus_path.c_str(), std::cout),
	_mih_user(cfg, io,
	          boost::bind(&NetworkManager::event_handler, this, _1, _2),
	          boost::bind(&NetworkManager::new_device, this, _1, _2))
{
	// FIXME
	NetworkManager_adaptor::State = NM_STATE_DISCONNECTED;//NM_STATE_UNKNOWN;
	NetworkManager_adaptor::Version = cfg.get<std::string>(kConf_Version);
	NetworkManager_adaptor::ActiveConnections = std::vector< ::DBus::Path >();
	NetworkManager_adaptor::WimaxHardwareEnabled = false;
	NetworkManager_adaptor::WimaxEnabled = false;
	NetworkManager_adaptor::WwanHardwareEnabled = false;
	NetworkManager_adaptor::WwanEnabled = false;
	NetworkManager_adaptor::WirelessHardwareEnabled = false;
	NetworkManager_adaptor::WirelessEnabled = false;
	NetworkManager_adaptor::NetworkingEnabled = true;
}

NetworkManager::~NetworkManager()
{
}

uint32_t NetworkManager::state()
{
	return NetworkManager_adaptor::State();
}

void NetworkManager::state(NM_STATE newstate)
{
	// store
	NetworkManager_adaptor::State = newstate;

	// signal
	NetworkManager_adaptor::StateChanged(State());
}

void NetworkManager::SetLogging(const std::string& level, const std::string& domains)
{
	// TODO
	// never?
}

std::map< std::string, std::string > NetworkManager::GetPermissions()
{
	log_(0, "Getting user permissions. Sending \"yes\" to all");

	std::map< std::string, std::string > r;

	r["org.freedesktop.NetworkManager.settings.modify.system"] = "yes";
	r["org.freedesktop.NetworkManager.wifi.share.protected"] = "yes";
	r["org.freedesktop.NetworkManager.wifi.share.open"] = "yes";
	r["org.freedesktop.NetworkManager.settings.modify.hostname"] = "yes";

	return r;
}

void NetworkManager::Enable(const bool& enable)
{
	NetworkingEnabled = enable;
	PropertiesChanged(map_list_of("NetworkingEnabled", to_variant(NetworkingEnabled())));

	if (!enable) {
		log_(0, "Disabling");

		state(NM_STATE_DISCONNECTING);

		for (auto it = _device_map.begin(); it != _device_map.end(); ++it) {
			it->second->Disable();
			clear_connections(it->first);
		}
	} else {
		log_(0, "Enabling");
		// TODO trigger a "start managing"
	}
}

void NetworkManager::Sleep(const bool& sleep)
{
	if (sleep) {
		log_(0, "Going to sleep");

		state(NM_STATE_DISCONNECTING);

		for (auto it = _device_map.begin(); it != _device_map.end(); ++it) {
			it->second->Disconnect();
			clear_connections(it->first);
		}

		state(NM_STATE_ASLEEP);
	} else {
		log_(0, "Waking up");
		// TODO trigger a "start managing"
	}
}

void NetworkManager::DeactivateConnection(const ::DBus::Path& active_connection)
{
	log_(0, "Deactivating connection");
	// TODO
}

void NetworkManager::AddAndActivateConnection(
	const settings_map& connection,
	const ::DBus::Path& device,
	const ::DBus::Path& specific_object,
	::DBus::Path& path,
	::DBus::Path& active_connection)
{
	log_(0, "Adding and activating connection");

	path = "/";
	active_connection = "/";

	try {
		// TODO check if device exists
		auto d = _device_map.find(device);
		if (d == _device_map.end()) {
			throw DBus::Error("org.freedesktop.NetworkManager.Error.UnknownDevice", "Unknown device");
		}

		switch (d->second->DeviceType()) {
			case Device::NM_DEVICE_TYPE_ETHERNET:
			{
				path = _settings.AddConnection(connection);
				active_connection = ActivateConnection(path, device, specific_object);
			}
			break;

			case Device::NM_DEVICE_TYPE_MODEM:
				break;
			case Device::NM_DEVICE_TYPE_WIFI:
			{
				// find a previous configuration with the given SSID
				std::shared_ptr<DeviceWireless> _d = std::static_pointer_cast<DeviceWireless>(d->second);
				AccessPoint::bss_id bss = _d->get_access_point(specific_object);

				// get the path of settings with such ssid
				boost::optional<DBus::Path> settings;
				std::vector<unsigned char> ssid_vector(bss.ssid.begin(), bss.ssid.end());
				settings = _settings.get_connection_by_attribute("802-11-wireless", "ssid", ssid_vector);

				if (settings) {
					path = settings.get();
				} else { // go ahead and use the provided one
					// add at least the SSID, since nm-applet won't do it
					settings_map sm = connection;
					sm["802-11-wireless"]["ssid"] = to_variant(ssid_vector);
					path = _settings.AddConnection(sm);
				}

				active_connection = ActivateConnection(path, device, specific_object);
			}
			break;

			case Device::NM_DEVICE_TYPE_WIMAX:
				break;
			default:
				log_(0, "Unsupported device type");
		}
		// TODO output/result parameters
	} catch (std::exception &e) {
		log_(0, "Exception: ", e.what());
		throw;
	}
}

::DBus::Path NetworkManager::ActivateConnection(
	const ::DBus::Path& connection,
	const ::DBus::Path& device,
	const ::DBus::Path& specific_object)
{
	log_(0, "Activating connection");

	state(NM_STATE_CONNECTING);

	::DBus::Path active_connection;

	try {
		// clear the active connections for this device
		clear_connections(device);

		settings_map settings = _settings.GetSettings(connection);
		std::string uuid = settings.find("connection")->second.find("uuid")->second;
		boost::algorithm::erase_all(uuid, "-"); // remove dashes
		active_connection = std::string(_dbus_path + "/ActiveConnections/" + uuid);

		std::vector<DBus::Path> devices;
		devices.push_back(device);

		// create a new active connection object for this device
		std::shared_ptr<ConnectionActive> connection_active(
			new ConnectionActive(_connection,
								 active_connection.c_str(),
								 connection,
								 specific_object,
								 uuid,
								 devices,
								 ConnectionActive::NM_ACTIVE_CONNECTION_STATE_ACTIVATING,
								 true, // TODO
								 true, // TODO
								 false
			));

		// add the new active connection
		_active_connections[active_connection] = connection_active;
		_device_active_connection[device] = active_connection;

		// signal the change
		std::vector<DBus::Path> active_connection_list = active_connections();
		ActiveConnections = active_connection_list;
		PropertiesChanged(map_list_of("ActiveConnections", to_variant(active_connection_list)));

		// TODO check if device exists
		auto d = _device_map.find(device);
		if (d == _device_map.end()) {
			throw DBus::Error("org.freedesktop.NetworkManager.Error.UnknownDevice", "Unknown device");
		}

		link_conf(device, active_connection);
	} catch (std::exception &e) {
		log_(0, "Exception: ", e.what());
		throw;
	}

	return active_connection;
}

::DBus::Path NetworkManager::GetDeviceByIpIface(const std::string& iface)
{
	log_(0, "Getting device by name (IpIface): ", iface);

	for (auto it = _device_map.begin(); it != _device_map.end(); ++it) {
		if (it->second->IpInterface() == iface) {
			log_(0, iface, " found");
			return it->first;
		}
	}

	log_(0, iface, " not found");
	throw DBus::Error("org.freedesktop.NetworkManager.UnknownDevice", "No device found for the requested iface.");
}

std::vector< ::DBus::Path > NetworkManager::GetDevices()
{
	log_(0, "Getting all devices");

	std::vector<DBus::Path> r;

	boost::copy(_device_map | boost::adaptors::map_keys, std::back_inserter(r));

	log_(0, "Done");
	return r;
}

void NetworkManager::new_device(mih::network_type &type, mih::link_addr &address)
{
	log_(0, "New device detected");

	mih::link_type ltype = boost::get<mih::link_type>(type.link);
	if (ltype == mih::link_type_802_11) {
		mih::mac_addr mac = boost::get<mih::mac_addr>(address);

		add_802_11_device(mac);
	} else if (ltype == mih::link_type_ethernet) {
		mih::mac_addr mac = boost::get<mih::mac_addr>(address);

		add_ethernet_device(mac);
	} else {
		log_(0, "Unsupported device type");
	}
}

void NetworkManager::add_802_11_device(mih::mac_addr &address)
{
	log_(0, "Adding WiFi device, address: ", address.address());

	mih::link_tuple_id lti;
	lti.addr = address;
	lti.type = mih::link_type_802_11;

	// determine the D-Bus path
	std::stringstream path;
	path << _dbus_path << "/Devices/" << boost::algorithm::erase_all_copy(address.address(), ":");

	std::shared_ptr<Device> d(
		new DeviceWireless(_connection, path.str().c_str(), _mih_user, lti));

	// if networking is disabled, shut this interface
	if (!NetworkManager_adaptor::NetworkingEnabled() || !NetworkManager_adaptor::WirelessEnabled()) {
		d->Disable();
	} else {
		// TODO
		// attempt to connect? if disconnected?
	}

	// save the device
	_device_map[DBus::Path(path.str())] = d;

	// signal new device
	NetworkManager_adaptor::DeviceAdded(path.str().c_str());
	log_(0, "Device added");

	// wireless network hardware is now enabled
	if (!NetworkManager_adaptor::WirelessHardwareEnabled()) {
		WirelessHardwareEnabled = true;
		PropertiesChanged(map_list_of("WirelessHardwareEnabled", to_variant(WirelessHardwareEnabled())));
	}
}

void NetworkManager::add_ethernet_device(mih::mac_addr &address)
{
	log_(0, "Adding Ethernet device, address: ", address.address());

	mih::link_tuple_id lti;
	lti.addr = address;
	lti.type = mih::link_type_ethernet;

	// determine the D-Bus path
	std::stringstream path;
	path << _dbus_path << "/Devices/" << boost::algorithm::replace_all_copy(address.address(), ":", "");

	std::shared_ptr<Device> d(
		new DeviceWired(_connection, path.str().c_str(), _mih_user, lti));

	// if networking is disabled, shut this interface
	if (!NetworkManager_adaptor::NetworkingEnabled()) {
		d->Disable();
	} else {
		d->Enable();
	}

	// save the device
	_device_map[DBus::Path(path.str())] = d;

	// signal new device
	NetworkManager_adaptor::DeviceAdded(path.str().c_str());
	log_(0, "Device added");
}

void NetworkManager::link_up(const mih::mac_addr &dev, const boost::optional<mih::mac_addr> &poa)
{
	log_(0, "New L2 completed for device ", dev.address(), " with poa ", poa ? poa.get().address() : "[none]");

	// look for device and inform/check
	bool match;
	for (auto it = _device_map.begin(); it != _device_map.end() && !match; ++it) {
		switch (it->second->DeviceType()) {
			case Device::NM_DEVICE_TYPE_WIFI:
			{
				std::shared_ptr<DeviceWireless> d = std::static_pointer_cast<DeviceWireless>(it->second);
				if (d->address() == dev.address()) {
					match = true;
					d->link_up(poa);
				}
			}
			break;

			case Device::NM_DEVICE_TYPE_ETHERNET:
			{
				std::shared_ptr<DeviceWired> d = std::static_pointer_cast<DeviceWired>(it->second);
				if (d->address() == dev.address()) {
					match = true;
					d->link_up(poa);
				}
			}
			break;

			case Device::NM_DEVICE_TYPE_WIMAX:
			{
				std::shared_ptr<DeviceWiMax> d = std::static_pointer_cast<DeviceWiMax>(it->second);
				if (d->address() == dev.address()) {
					match = true;
					d->link_up(poa);
				}
			}
			break;

/*			case Device::NM_DEVICE_TYPE_BT:
			case Device::NM_DEVICE_TYPE_OLPC_MESH:
			case Device::NM_DEVICE_TYPE_INFINIBAND:
			case Device::NM_DEVICE_TYPE_BOND:
			case Device::NM_DEVICE_TYPE_VLAN:*/
			default:
				// unsupported
				break;
		}
	}
}

void NetworkManager::link_down(const mih::mac_addr &dev)
{
	log_(0, "L2 connection dropped on device ", dev.address());

	// update the device and connection information
	for (auto it = _device_map.begin(); it != _device_map.end(); ++it) {
		if (!boost::iequals(it->second->address(), dev.address())) {
			continue;
		}

		// if the disconnect was deliberate, cleanup the ConnectionActive objects
		if (   it->second->State() < Device::NM_DEVICE_STATE_PREPARE
		    || !NetworkingEnabled()
			|| (!WirelessEnabled() && it->second->DeviceType() == Device::NM_DEVICE_TYPE_WIFI)) {
				// there's another, untestable condition, which is power off on the hardware
			clear_connections(it->first);
		} else {
			DBus::Path active_connection_path = it->second->ActiveConnection();
			auto active_connection_it = _active_connections.find(active_connection_path);
			if (active_connection_it != _active_connections.end()) {
				active_connection_it->second->state(ConnectionActive::NM_ACTIVE_CONNECTION_STATE_UNKNOWN);
			}

			// TODO reconnect, or simply wait!!!
		}

		it->second->link_down();

		break;
	}

	// update the NetworkManager state
	NM_STATE newstate = NM_STATE_DISCONNECTED;

	// if the current state is connected or connecting, check for still existing ConnectionActive
	if (State() > NM_STATE_DISCONNECTED) {
		for (auto it = _active_connections.begin();
		     it != _active_connections.end() && newstate != NM_STATE_CONNECTED_GLOBAL;
		     ++it) {
			// upgrade to STATE_CONNECTING if any ACTIVATING found,
			// and to CONNECTED_GLOBAL if any ACTIVATED found.
			if (it->second->State() == ConnectionActive::NM_ACTIVE_CONNECTION_STATE_ACTIVATING) {
				newstate = NM_STATE_CONNECTING;
			} else if (it->second->State() == ConnectionActive::NM_ACTIVE_CONNECTION_STATE_ACTIVATED) {
				newstate = NM_STATE_CONNECTED_GLOBAL;

				// restore IP configuration for this connection
				if (it->second->Devices().size() > 0) {
					l3_conf(it->second->Devices()[0], it->first);
				}
			}
		}
	}
	state(newstate);
}

void NetworkManager::links_detected(const std::vector<mih::link_det_info> &ldil)
{
	// tell DeviceWireless to update the list
	// TODO target each specific DeviceWireless
	BOOST_FOREACH (mih::link_det_info ldi, ldil) {
		mih::mac_addr dev_addr = boost::get<mih::mac_addr>(ldi.id.addr);

		for (auto it = _device_map.begin(); it != _device_map.end(); ++it) {
			if (it->second->DeviceType() == Device::NM_DEVICE_TYPE_WIFI) {
				std::shared_ptr<DeviceWireless> d = std::static_pointer_cast<DeviceWireless>(it->second);
				if (boost::iequals(d->address(), dev_addr.address())) {
					d->add_ap(ldi);
				}
			}
		}
	}
}

void NetworkManager::parameters_report(const mih::link_tuple_id &lti,
                                       const mih::link_param_rpt_list &rpt_list)
{
	std::string device_address = boost::get<mih::mac_addr>(lti.addr).address();

	for (auto it = _device_map.begin(); it != _device_map.end(); ++it) {
		if (boost::iequals(it->second->address(), device_address)) {
			it->second->parameters_report(rpt_list);
			break;
		}
	}
}

void NetworkManager::on_set_property(DBus::InterfaceAdaptor &interface,
                                     const std::string &prop,
                                     const DBus::Variant &value)
{
	log_(0, "Setting property \"", prop, "\"");

	if (boost::iequals(prop, "WirelessEnabled")) {
		for (auto it = _device_map.begin(); it != _device_map.end(); ++it) {
			if (it->second->DeviceType() == Device::NM_DEVICE_TYPE_WIFI) {
				if (!value) {
					clear_connections(it->first);
					it->second->Disable();//Disconnect();
				} else {
					it->second->Enable();
				}
				WirelessEnabled = static_cast<bool>(value);
				PropertiesChanged(map_list_of("WirelessEnabled", to_variant(WirelessEnabled())));
			}
		}
	} else if (boost::iequals(prop, "WwanEnabled")) {
		// Unsupported
	} else if (boost::iequals(prop, "WimaxEnabled")) {
		// Unsupported
	}

	// TODO: check if no other connection active and change state

	PropertiesAdaptor::on_set_property(interface, prop, value);
}

std::vector<DBus::Path> NetworkManager::active_connections()
{
	std::vector<DBus::Path> r;

	boost::copy(_active_connections | boost::adaptors::map_keys, std::back_inserter(r));

	return r;
}

// TODO move this into the device!
// this should only merge the settings and then command the Device!
void NetworkManager::link_conf(const DBus::Path &device, const DBus::Path &connection_active)
{
	auto dev = _device_map.find(device);
	if (dev == _device_map.end()) {
		throw std::runtime_error("Device not found");
	}

	auto connection_active_it = _active_connections.find(connection_active);
	if (connection_active_it == _active_connections.end()) {
		throw std::runtime_error("Unkonwn ConnectionActive \"" + connection_active + "\"");
	}
	DBus::Path connection = connection_active_it->second->Connection();
	DBus::Path specific_object = connection_active_it->second->SpecificObject();

	log_(0, "Configuration found");

	std::map<std::string, std::string> conf;
	conf = _settings.wpa_conf(connection);

	mih::configuration_list confl;
	for (auto it = conf.begin(); it != conf.end(); ++it) {
		mih::configuration c;
		c.key = it->first;
		c.value = it->second;
		confl.push_back(c);
	}

	boost::optional<mih::link_addr> network; // fetch from specific object

	// respond
	dev->second->link_conf(
		[&, device, connection_active](bool success) {
			if (success) {
				log_(0, "Authentication success");
				l3_conf(device, connection_active);
			} else {
				log_(0, "Authentication failure");
				clear_connections(device);
				// TODO?
			}
		}, network, confl, connection_active, specific_object);
}

void NetworkManager::l3_conf(const DBus::Path &device, const DBus::Path &connection_active)
{
	mih::ip_cfg_methods cfg_methods;
	mih::ip_info_list address_list;
	mih::ip_info_list route_list;
	mih::ip_addr_list dns_list;
	mih::fqdn_list domain_list;

	// get the device
	auto dev = _device_map.find(device);
	if (dev == _device_map.end()) {
		throw std::runtime_error("Device not found \"" + device + "\"");
	}

	// get the connection
	auto connection_active_it = _active_connections.find(connection_active);
	if (connection_active_it == _active_connections.end()) {
		throw std::runtime_error("ConnectionActive not found \"" + connection_active + "\"");
	}
	DBus::Path connection = connection_active_it->second->Connection();

	//
	// Parse IPv4 configurations
	//
	settings_map settings = _settings.GetSettings(connection);
	auto v4_settings_it = settings.find("ipv4");
	if (v4_settings_it != settings.end()) {
		log_(0, "Parsing IPv4 settings");

		// Parse the configuration method
		auto method_it = v4_settings_it->second.find("method");
		if (method_it == v4_settings_it->second.end()) {
			throw std::runtime_error("ipv4 settings are missing a \"method\" setting.");
		}

		std::string method = from_variant<std::string>(method_it->second);
		if (boost::iequals(method, "auto")) {
			cfg_methods.set(mih::ip_cfg_ipv4_dynamic);
		} else if (boost::iequals(method, "manual")) {
			cfg_methods.set(mih::ip_cfg_ipv4_static);
		} else if (boost::iequals(method, "disabled")) {
			// do nothing
		//} else if (boost::iequals(method, "shared")) {
		//	// assign a 10.42.x.1/24 and start a DHCP and forwarding DNS server
		//} else if (boost::iequals(method, "link-local")) {
		//	// assign a 169.254/16 address
		} else {
			log_(0, "Unsupported ipv4 method specified");
			throw std::runtime_error("Unsupported ipv4 method specified");
		}

		// Parse the specific addresses field
		auto addresses_it = v4_settings_it->second.find("addresses");
		if (addresses_it != v4_settings_it->second.end()) {
			log_(0, "Parsing IPv4 addresses");

			cfg_methods.set(mih::ip_cfg_ipv4_static);

			std::vector<std::vector<uint32_t>> addresses;
			addresses = from_variant<std::vector<std::vector<uint32_t>>>(addresses_it->second);
			for (unsigned int r = 0; r < addresses.size(); ++r) {
				mih::ip_info info;
				info.subnet.ipaddr = mih::ip_addr(mih::ip_addr::ipv4, &addresses[r][0], 4);
				info.subnet.ipprefixlen = addresses[r][1];
				info.gateway = mih::ip_addr(mih::ip_addr::ipv4, &addresses[r][2], 4);

				address_list.push_back(info);
			}
		}

		// Parse the specific routes field
		auto routes_it = v4_settings_it->second.find("routes");
		if (routes_it != v4_settings_it->second.end()) {
			log_(0, "Parsing IPv4 routes");

			cfg_methods.set(mih::ip_cfg_ipv4_static);

			std::vector<std::vector<uint32_t>> routes;
			routes = from_variant<std::vector<std::vector<uint32_t>>>(routes_it->second);
			for (unsigned int r = 0; r < routes.size(); ++r) {
				mih::ip_info info;
				info.subnet.ipaddr = mih::ip_addr(mih::ip_addr::ipv4, &routes[r][0], 4);
				info.subnet.ipprefixlen = routes[r][1];
				info.gateway = mih::ip_addr(mih::ip_addr::ipv4, &routes[r][2], 4);

				route_list.push_back(info);
			}
		}

		// Parse the specific dns servers field
		auto dns_it = v4_settings_it->second.find("dns");
		if (dns_it != v4_settings_it->second.end()) {
			log_(0, "Parsing IPv4 dns");

			cfg_methods.set(mih::ip_cfg_ipv4_static);

			std::vector<uint32_t> nameservers = from_variant<std::vector<uint32_t>>(dns_it->second);
			for (unsigned int r = 0; r < nameservers.size(); ++r) {
				dns_list.push_back(mih::ip_addr(mih::ip_addr::ipv4, &nameservers[r], 4));
			}
		}

		// Parse the specific dns-search field
		auto domains_it = v4_settings_it->second.find("dns-search");
		if (domains_it != v4_settings_it->second.end()) {
			log_(0, "Parsing IPv4 domains");

			cfg_methods.set(mih::ip_cfg_ipv4_static);

			std::vector<std::string> nameservers = from_variant<std::vector<std::string>>(domains_it->second);
			domain_list.insert(domain_list.end(), nameservers.begin(), nameservers.end());
		}
	}

	//
	// Parse IPv6 configurations
	//
	auto v6_settings_it = settings.find("ipv6");
	if (v6_settings_it != settings.end()) {
		log_(0, "Parsing IPv6 settings");

		// Parse the configuration method
		auto method_it = v6_settings_it->second.find("method");
		if (method_it == v6_settings_it->second.end()) {
			throw std::runtime_error("ipv6 settings are missing a \"method\" setting.");
		}

		std::string method = from_variant<std::string>(method_it->second);
		if (boost::iequals(method, "auto") || boost::iequals(method, "dhcp")) {
			cfg_methods.set(mih::ip_cfg_ipv6_stateful);
		} else if (boost::iequals(method, "manual")) {
			cfg_methods.set(mih::ip_cfg_ipv6_manual);
		} else if (boost::iequals(method, "ignore")) {
			// do nothing
		//} else if (boost::iequals(method, "link-local")) {
		//	// assign a link-local address
		} else {
			log_(0, "Unsupported ipv6 method specified");
			throw std::runtime_error("Unsupported ipv6 method specified: \"" + method + "\"");
		}

		// Parse the specific addresses field
		auto addresses_it = v6_settings_it->second.find("addresses");
		if (addresses_it != v6_settings_it->second.end()) {
			log_(0, "Parsing IPv6 addresses");

			cfg_methods.set(mih::ip_cfg_ipv6_manual);

			std::vector<ip6_addr_tuple> addresses;
			addresses = from_variant<std::vector<ip6_addr_tuple>>(addresses_it->second);
			for (unsigned int r = 0; r < addresses.size(); ++r) {
				mih::ip_info info;
				info.subnet.ipaddr = mih::ip_addr(mih::ip_addr::ipv6, &addresses[r]._1[0], 16);
				info.subnet.ipprefixlen = addresses[r]._2;
				info.gateway = mih::ip_addr(mih::ip_addr::ipv6, &addresses[r]._3[0], 16);

				address_list.push_back(info);
			}
		}

		// Parse the specific routes field
		auto routes_it = v6_settings_it->second.find("routes");
		if (routes_it != v6_settings_it->second.end()) {
			log_(0, "Parsing IPv6 routes");

			cfg_methods.set(mih::ip_cfg_ipv6_manual);

			std::vector<ip6_route_tuple> routes;
			routes = from_variant<std::vector<ip6_route_tuple>>(routes_it->second);
			for (unsigned int r = 0; r < routes.size(); ++r) {
				mih::ip_info info;
				info.subnet.ipaddr = mih::ip_addr(mih::ip_addr::ipv6, &routes[r]._1[0], 16);
				info.subnet.ipprefixlen = routes[r]._2;
				info.gateway = mih::ip_addr(mih::ip_addr::ipv6, &routes[r]._3[0], 16);

				route_list.push_back(info);
			}
		}

		// Parse the specific dns servers field
		auto dns_it = v6_settings_it->second.find("dns");
		if (dns_it != v6_settings_it->second.end()) {
			log_(0, "Parsing IPv6 dns");

			cfg_methods.set(mih::ip_cfg_ipv6_manual);

			std::vector<std::vector<uint8_t>> nameservers;
			nameservers = from_variant<std::vector<std::vector<uint8_t>>>(dns_it->second);
			for (unsigned int r = 0; r < nameservers.size(); ++r) {
				dns_list.push_back(mih::ip_addr(mih::ip_addr::ipv6, &nameservers[r][0], 16));
			}
		}

		// Parse the specific dns-search field
		auto domains_it = v6_settings_it->second.find("dns-search");
		if (domains_it != v6_settings_it->second.end()) {
			log_(0, "Parsing IPv6 domains");

			cfg_methods.set(mih::ip_cfg_ipv6_manual);

			std::vector<std::string> nameservers;
			nameservers = from_variant<std::vector<std::string>>(domains_it->second);
			domain_list.insert(domain_list.end(), nameservers.begin(), nameservers.end());
		}
	}

	dev->second->l3_conf(
		[&, device, connection_active](bool success) {
			if (success) {
				log_(0, "Success configuring L3");

				auto dev = _device_map.find(device);
				if (dev == _device_map.end()) {
					throw std::runtime_error("Device object not found \"" + device + "\"");
				}
				dev->second->l3_up();

				auto active_connection_it = _active_connections.find(connection_active);
				if (active_connection_it == _active_connections.end()) {
					throw std::runtime_error("No ConnectionActive object created for this state");
				}
				active_connection_it->second->state(ConnectionActive::NM_ACTIVE_CONNECTION_STATE_ACTIVATED);

				state(NM_STATE_CONNECTED_GLOBAL);
			} else {
				log_(0, "Error configuring L3");
				clear_connections(device);
				// TODO?
			}
		},
		cfg_methods,
		boost::make_optional(address_list.size() > 0, address_list),
		boost::make_optional(route_list.size()   > 0, route_list),
		boost::make_optional(dns_list.size()     > 0, dns_list),
		boost::make_optional(domain_list.size()  > 0, domain_list));
}

void NetworkManager::clear_connections(const DBus::Path &device)
{
	// WARNING when changing this method, check for the effects on every usage!

	// clear the active connections for this device
	auto ac = _device_active_connection.find(device);
	if (ac != _device_active_connection.end()) {
		_active_connections.erase(_active_connections.find(ac->second));
		_device_active_connection.erase(ac);

		// signal the change
		std::vector<DBus::Path> active_connection_list = active_connections();
		ActiveConnections = active_connection_list;
		PropertiesChanged(map_list_of("ActiveConnections", to_variant(ActiveConnections())));
	}
}

void NetworkManager::event_handler(mih::message &msg, const boost::system::error_code &ec)
{
	if (ec) {
		log_(0, "Error event received, status: ", ec.message());
		return;
	}

	switch (msg.mid()) {
	case mih::indication::link_up:
	{
		log_(0, "Received a link_up event");

		mih::link_tuple_id up_link;
		boost::optional<mih::link_addr> __old_rout, __new_rout;
		boost::optional<bool> __ip_renew;
		boost::optional<mih::ip_mob_mgmt> __ip_mob;

		msg >> mih::indication()
			& mih::tlv_link_identifier(up_link)
			& mih::tlv_old_access_router(__old_rout)
			& mih::tlv_new_access_router(__new_rout)
			& mih::tlv_ip_renewal_flag(__ip_renew)
			& mih::tlv_ip_mob_mgmt(__ip_mob);

		mih::mac_addr *dev = boost::get<mih::mac_addr>(&up_link.addr);
		if (!dev) {
			log_(0, "Unable to determine device address.");
			break;
		}

		boost::optional<mih::mac_addr> opoa;
		mih::mac_addr *poa;
		mih::link_addr *_poa = boost::get<mih::link_addr>(&up_link.poa_addr);
		if (_poa) {
			poa = boost::get<mih::mac_addr>(_poa);
			if (!poa) {
				log_(0, "Unable to determine poa address");
			} else {
				opoa = *poa;
			}
		} else {
			log_(0, "Unable to determine poa address");
		}

		_io.dispatch(boost::bind(&NetworkManager::link_up, this, *dev, opoa));
	}
	break;

	case mih::indication::link_down:
	{
		log_(0, "Received a link_down event");

		mih::link_tuple_id down_link;
		boost::optional<mih::link_addr> __old_rout;
		mih::link_dn_reason __dn_reason;

		msg >> mih::indication()
			& mih::tlv_link_identifier(down_link)
			& mih::tlv_old_access_router(__old_rout)
			& mih::tlv_link_dn_reason(__dn_reason);

		mih::mac_addr *dev = boost::get<mih::mac_addr>(&down_link.addr);
		if (!dev) {
			log_(0, "Unable to determine device address.");
			break;
		}

		_io.dispatch(boost::bind(&NetworkManager::link_down, this, *dev));
	}
	break;

	case mih::indication::link_detected:
	{
		log_(0, "Received a link_detected event");

		std::vector<mih::link_det_info> ldil;

		msg >> mih::indication()
			& mih::tlv_link_det_info_list(ldil);

		_io.dispatch(boost::bind(&NetworkManager::links_detected, this, ldil));
	}
	break;

	case mih::indication::link_parameters_report:
	{
		log_(0, "Received a link_parameters report");

		mih::link_tuple_id lti;
		mih::link_param_rpt_list rpt_list;

		msg >> mih::indication()
			& mih::tlv_link_identifier(lti)
			& mih::tlv_link_param_rpt_list(rpt_list);

		_io.dispatch(boost::bind(&NetworkManager::parameters_report, this, lti, rpt_list));
	}
	break;

	case mih::indication::link_going_down:
		// TODO
		log_(0, "Received a link_going_down event");
		break;

	case mih::indication::link_handover_imminent:
		log_(0, "Received a link_handover_imminent event");
		break;

	case mih::indication::link_handover_complete:
		log_(0, "Received a link_handover_complete event");
		break;

	case mih::indication::link_conf_required:
	{
		log_(0, "Received a link_conf_required event");

		mih::link_tuple_id lti;
		boost::optional<mih::link_addr> poa;
		mih::configuration_list lconf;

		msg >> mih::indication()
			& mih::tlv_link_identifier(lti)
			& mih::tlv_poa(poa)
			& mih::tlv_configuration_list(lconf);

		mih::mac_addr address = boost::get<mih::mac_addr>(lti.addr);
		std::stringstream path;
		path << _dbus_path << "/Devices/" << boost::algorithm::erase_all_copy(address.address(), ":");
		DBus::Path device = path.str();

		boost::optional<DBus::Path> connection;
		// look for ongoing progress of a connection on this link!
		auto ac = _device_active_connection.find(device);
		if (ac != _device_active_connection.end()) {
			_io.dispatch(boost::bind(&NetworkManager::link_conf, this, device, ac->first));
		} else {
			throw std::runtime_error("No ongoing connection on this link");
			// TODO this is from
		}
	}
	break;

	default:
		log_(0, "Received unknown/unsupported event");
	}
}
