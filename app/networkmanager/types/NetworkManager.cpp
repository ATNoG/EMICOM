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

using namespace odtone::networkmanager;

NetworkManager::NetworkManager(DBus::Connection &connection, const mih::config &cfg, boost::asio::io_service &io) :
	DBus::ObjectAdaptor(connection, cfg.get<std::string>(kConf_DBus_Path).c_str()),
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
	NetworkManager_adaptor::StateChanged(newstate);
}

/**
 * ActiveConnections is a special case and "value" is ignored.
 * The list is fetched from the map!
 */
template <class T>
void NetworkManager::property(const std::string &property, const T &value)
{
/*	if (boost::iequals(property, "NetworkingEnabled")) {
		NetworkManager_adaptor::NetworkingEnabled = value;
	} else if (boost::iequals(property, "WirelessEnabled")) {
		NetworkManager_adaptor::WirelessEnabled = value;
	} else if (boost::iequals(property, "WirelessHardwareEnabled")) {
		NetworkManager_adaptor::WirelessHardwareEnabled = value;
	} else if (boost::iequals(property, "WwanEnabled")) {
		NetworkManager_adaptor::WwanEnabled = value;
	} else if (boost::iequals(property, "WwanHardwareEnabled")) {
		NetworkManager_adaptor::WwanHardwareEnabled = value;
	} else if (boost::iequals(property, "WimaxEnabled")) {
		NetworkManager_adaptor::WimaxEnabled = value;
	} else if (boost::iequals(property, "WimaxHardwareEnabled")) {
		NetworkManager_adaptor::WimaxHardwareEnabled = value;
	} else if (boost::iequals(property, "ActiveConnections")) {
		NetworkManager_adaptor::ActiveConnections = value;
	} else if (boost::iequals(property, "Version")) {
		NetworkManager_adaptor::Version = value;
	} else if (boost::iequals(property, "State")) {
		NetworkManager_adaptor::State = value;
	//} else {
	//	// fail
	}*/

	std::map<std::string, DBus::Variant> props;
	props[property] = to_variant(value);
	NetworkManager_adaptor::PropertiesChanged(props);
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
	NetworkManager_adaptor::NetworkingEnabled = enable;

	if (!enable) {
		log_(0, "Disabling");

		state(NM_STATE_DISCONNECTING);

		auto it = _device_map.begin();
		while (it != _device_map.end()) {
			it->second->Disconnect();
			it++;
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

		auto it = _device_map.begin();
		while (it != _device_map.end()) {
			it->second->Disconnect();
			it++;
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

/*	// debug info
	std::cerr << "settings:" << std::endl;
	auto a = connection.begin();
	while (a != connection.end()) {
		std::cerr << a->first << " {" << std::endl;
		auto b = a->second.begin();
		while (b != a->second.end()) {
			std::string key = b->first;
			std::string value = b->second.signature();
			if (value == "s") {
				std::string val = b->second;
				value = val;
			}
			std::cerr << key << ": " << value << std::endl;
			b ++;
		}
		std::cerr << "}" << std::endl;
		a ++;
	}

	std::cerr << "device: " << device << std::endl;
	std::cerr << "specific object: " << specific_object << std::endl;*/

	state(NM_STATE_CONNECTING);

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
//				// if there's no indication of SSID, try to find settings that do
//				auto s = connection.find("802-11-wireless");
//				if (s == connection.end() || s->second.find("ssid") == s->second.end()) {
					// find a previous configuration with the given SSID
					// get the ssid
					std::shared_ptr<DeviceWireless> _d = std::static_pointer_cast<DeviceWireless>(d->second);
					AccessPoint::bss_id bss = _d->get_access_point(specific_object);
					// get the path of settings with such ssid
					boost::optional<DBus::Path> settings;
					std::vector<unsigned char> ssid_vector(bss.ssid.begin(), bss.ssid.end());
					settings = _settings.get_connection_by_attribute("802-11-wireless", "ssid", ssid_vector);
					if (settings) {
						path = settings.get();
					}
//				}
				if (boost::iequals(path, "/")) { // go ahead and use the provided one
					path = _settings.AddConnection(connection);
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

	::DBus::Path active_connection;

	try {
		settings_map settings = _settings.GetSettings(connection);
		std::string uuid = settings.find("connection")->second.find("uuid")->second;
		boost::algorithm::erase_all(uuid, "-"); // remove dashes
		active_connection = std::string(_dbus_path + "/ActiveConnections/" + uuid);

		std::vector<DBus::Path> devices;
		devices.push_back(device);

		// clear the active connections for this device
		auto previous_active_connections = _active_connections.find(device);
		if (previous_active_connections != _active_connections.end()) {
			_active_connections.erase(previous_active_connections);
			std::vector<DBus::Path> active_connection_list = active_connections();
			ActiveConnections = active_connection_list;
			property("ActiveConnections", active_connection_list);
		}

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

		_active_connections[device][active_connection] = connection_active;
		std::vector<DBus::Path> active_connection_list = active_connections();
		ActiveConnections = active_connection_list;
		property("ActiveConnections", active_connection_list);

		// TODO check if device exists
		auto d = _device_map.find(device);
		if (d == _device_map.end()) {
			throw DBus::Error("org.freedesktop.NetworkManager.Error.UnknownDevice", "Unknown device");
		}

		switch (d->second->DeviceType()) {
			case Device::NM_DEVICE_TYPE_ETHERNET:
			{
				// connect
				d->second->Enable();
			}
			break;

			case Device::NM_DEVICE_TYPE_MODEM:
				break;
			case Device::NM_DEVICE_TYPE_WIFI:
			{
				// connect
				std::static_pointer_cast<DeviceWireless>(d->second)->Connect(specific_object,
					[&, device, connection](bool success) {
						if (!success) {
							log_(0, "Connection failed");
							state(NM_STATE_DISCONNECTED); // TODO check other interfaces
							return;
						}

						log_(0, "Authenticated, now configuring IP address");
						l3_conf(device, connection);

						state(NM_STATE_CONNECTING); // TODO check other interfaces
					});
			}
			break;

			case Device::NM_DEVICE_TYPE_WIMAX:
				break;
			default:
				log_(0, "Unsupported device type");
		}
	} catch (std::exception &e) {
		log_(0, "Exception: ", e.what());
		throw;
	}

	return active_connection;
}

::DBus::Path NetworkManager::GetDeviceByIpIface(const std::string& iface)
{
	log_(0, "Getting device by name (IpIface): ", iface);

	auto it = _device_map.begin();
	while (it != _device_map.end()) {
		if (it->second->IpInterface() == iface) {
			log_(0, iface, " found");
			return it->first;
		}
		it++;
	}

	log_(0, iface, " not found");
	throw DBus::Error("org.freedesktop.NetworkManager.UnknownDevice", "No device found for the requested iface.");
}

std::vector< ::DBus::Path > NetworkManager::GetDevices()
{
	log_(0, "Getting all devices");

	std::vector< ::DBus::Path > r;

	auto it = _device_map.begin();
	while (it != _device_map.end()) {
		r.push_back(it->first);
		it++;
	}

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
		property("WirelessHardwareEnabled", true);
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
		//d->Enable();
#warning fix this!
		d->Disable();
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
	auto it = _device_map.begin();
	while (it != _device_map.end() && !match) {
		switch (it->second->DeviceType()) {
			case Device::NM_DEVICE_TYPE_WIFI:
			{
				std::shared_ptr<DeviceWireless> d = std::static_pointer_cast<DeviceWireless>(it->second);
				if (d->HwAddress() == dev.address()) {
					match = true;
					d->link_up(poa);
				}
			}
			break;

			case Device::NM_DEVICE_TYPE_ETHERNET:
			{
				std::shared_ptr<DeviceWired> d = std::static_pointer_cast<DeviceWired>(it->second);
				if (d->HwAddress() == dev.address()) {
					match = true;
					d->link_up(poa);
				}
			}
			break;

			case Device::NM_DEVICE_TYPE_WIMAX:
			{
				std::shared_ptr<DeviceWiMax> d = std::static_pointer_cast<DeviceWiMax>(it->second);
				if (d->HwAddress() == dev.address()) {
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

		it++;
	}
}

void NetworkManager::link_down(const mih::mac_addr &dev)
{
	log_(0, "L2 connection dropped on device ", dev.address());

	// look for device and inform/check
	bool match = false;
	auto it = _device_map.begin();
	while (it != _device_map.end() && !match) {
		switch (it->second->DeviceType()) {
			case Device::NM_DEVICE_TYPE_WIFI:
			{
				std::shared_ptr<DeviceWireless> d = std::static_pointer_cast<DeviceWireless>(it->second);
				if (d->HwAddress() == dev.address()) {
					match = true;
					d->link_down();
				}
			}
			break;

			case Device::NM_DEVICE_TYPE_ETHERNET:
			{
				std::shared_ptr<DeviceWired> d = std::static_pointer_cast<DeviceWired>(it->second);
				if (d->HwAddress() == dev.address()) {
					match = true;
					d->link_down();
				}
			}
			break;

			case Device::NM_DEVICE_TYPE_WIMAX:
			{
				std::shared_ptr<DeviceWiMax> d = std::static_pointer_cast<DeviceWiMax>(it->second);
				if (d->HwAddress() == dev.address()) {
					match = true;
					d->link_down();
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

		it++;
	}

	// TODO
	// if the device is available and a connection was active, attemp reconnection
}

void NetworkManager::on_set_property(DBus::InterfaceAdaptor &interface,
                                     const std::string &prop,
                                     const DBus::Variant &value)
{
	log_(0, "Setting property \"", prop, "\"");

	if (boost::iequals(prop, "WirelessEnabled")) {
		auto it = _device_map.begin();
		while (it != _device_map.end()) {
			if (it->second->DeviceType() == Device::NM_DEVICE_TYPE_WIFI) {
				if (!value) {
					it->second->Disable();//Disconnect();
				} else {
					it->second->Enable();
				}
				WirelessEnabled = static_cast<bool>(value);
				property("WirelessEnabled", static_cast<bool>(value));
			}
			it ++;
		}
	} else if (boost::iequals(prop, "WwanEnabled")) {
		// Unsupported
	} else if (boost::iequals(prop, "WimaxEnabled")) {
		// Unsupported
	}

	PropertiesAdaptor::on_set_property(interface, prop, value);
}

std::vector<DBus::Path> NetworkManager::active_connections()
{
	std::vector<DBus::Path> keys;

	auto it = _active_connections.begin();
	while (it != _active_connections.end()) {
		boost::copy(it->second | boost::adaptors::map_keys, std::back_inserter(keys));
		it ++;
	}

	return keys;
}

void NetworkManager::l3_conf(const DBus::Path &device, const DBus::Path &connection)
{
	mih::ip_cfg_methods cfg_methods;
	mih::ip_info_list address_list;
	mih::ip_info_list route_list;
	mih::ip_addr_list dns_list;
	mih::fqdn_list domain_list;

	// get the device
	auto dev = _device_map.find(device);
	if (dev == _device_map.end()) {
		throw std::runtime_error("Device not found");
	}

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
		} else if (boost::iequals(method, "ignored")) {
			// do nothing
		//} else if (boost::iequals(method, "link-local")) {
		//	// assign a link-local address
		} else {
			log_(0, "Unsupported ipv6 method specified");
			throw std::runtime_error("Unsupported ipv6 method specified");
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
		[&](bool success) {
			if (success) {
				log_(0, "Success configuring L3");
			} else {
				log_(0, "Error configuring L3");
			}
		},
		cfg_methods,
		boost::make_optional(address_list.size() > 0, address_list),
		boost::make_optional(route_list.size()   > 0, route_list),
		boost::make_optional(dns_list.size()     > 0, dns_list),
		boost::make_optional(domain_list.size()  > 0, domain_list));
}

// TODO move this into the device!
// this should only merge the settings and then command the Device!
void NetworkManager::link_conf(const DBus::Path &device,
                               const boost::optional<mih::network_id> &network,
                               const mih::configuration_list &lconf)
{
	auto dev = _device_map.find(device);
	if (dev == _device_map.end()) {
		throw std::runtime_error("Device not found");
	}

	if (!(network && dev->second->DeviceType() == Device::NM_DEVICE_TYPE_WIFI)) {
		log_(0, "Network name required for 802_11 configuration");
		return;
	}

	// search by 802-11-wireless ssid
	std::vector<unsigned char> ssid(network.get().begin(), network.get().end());

	boost::optional<DBus::Path> connection;
	connection = _settings.get_connection_by_attribute("802-11-wireless", "ssid", ssid);

	if (!connection) {
		// request user input
		log_(0, "No configuration found, user input required");
		return;
	}

	log_(0, "Configuration found");

	std::map<std::string, std::string> conf;
	conf = _settings.wpa_conf(connection.get());

	mih::configuration_list confl;

	auto it = conf.begin();
	while (it != conf.end()) {
		mih::configuration c;
		c.key = it->first;
		c.value = it->second;
		confl.push_back(c);

		it ++;
	}

	// respond
	dev->second->link_conf(
		[&](bool success) {
			if (success) {
				log_(0, "Authentication success");
			} else {
				log_(0, "Authentication failure");
			}
		}, network, confl);
}

void NetworkManager::event_handler(mih::message &msg, const boost::system::error_code &ec)
{
	log_(0, "Event received, status: ", ec.message());

	if (ec) {
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

		link_up(*dev, opoa);
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

		link_down(*dev);
	}
	break;

	case mih::indication::link_detected:
	{
		log_(0, "Received a link_detected event");

		std::vector<mih::link_det_info> ldil;

		msg >> mih::indication()
			& mih::tlv_link_det_info_list(ldil);

		// tell DeviceWireless to update the list
		// TODO target each specific DeviceWireless
		BOOST_FOREACH (mih::link_det_info ldi, ldil) {
			mih::mac_addr dev_addr = boost::get<mih::mac_addr>(ldi.id.addr);

			auto it = _device_map.begin();
			while (it != _device_map.end()) {
				if (it->second->DeviceType() == Device::NM_DEVICE_TYPE_WIFI) {
					std::shared_ptr<DeviceWireless> d = std::static_pointer_cast<DeviceWireless>(it->second);
					if (boost::iequals(d->HwAddress(), dev_addr.address())) {
						d->add_ap(ldi);
					}
				}
				it++;
			}
		}
	}
	break;

	case mih::indication::link_parameters_report:
		log_(0, "Received a link_parameters report");
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
		boost::optional<mih::network_id> network;
		mih::configuration_list lconf;

		msg >> mih::indication()
			& mih::tlv_link_identifier(lti)
			& mih::tlv_network_id(network)
			& mih::tlv_configuration_list(lconf);

		mih::mac_addr address = boost::get<mih::mac_addr>(lti.addr);
		std::stringstream path;
		path << _dbus_path << "/Devices/" << boost::algorithm::erase_all_copy(address.address(), ":");

		link_conf(path.str(), network, lconf);
	}
	break;

	default:
		log_(0, "Received unknown/unsupported event");
	}
}
