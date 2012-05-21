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

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

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

template <class T>
DBus::Variant NetworkManager::to_variant(T value)
{
	DBus::Variant v;
	DBus::MessageIter iter = v.writer();
	iter << value;
	return v;
}

void NetworkManager::property(const std::string &property, const DBus::Variant &value)
{
	if (boost::iequals(property, "NetworkingEnabled")) {
		NetworkManager_adaptor::NetworkingEnabled = static_cast<bool>(value);
	} else if (boost::iequals(property, "WirelessEnabled")) {
		NetworkManager_adaptor::WirelessEnabled = static_cast<bool>(value);
	} else if (boost::iequals(property, "WirelessHardwareEnabled")) {
		NetworkManager_adaptor::WirelessHardwareEnabled = static_cast<bool>(value);
	} else if (boost::iequals(property, "WwanEnabled")) {
		NetworkManager_adaptor::WwanEnabled = static_cast<bool>(value);
	} else if (boost::iequals(property, "WwanHardwareEnabled")) {
		NetworkManager_adaptor::WwanHardwareEnabled = static_cast<bool>(value);
	} else if (boost::iequals(property, "WimaxEnabled")) {
		NetworkManager_adaptor::WimaxEnabled = static_cast<bool>(value);
	} else if (boost::iequals(property, "WimaxHardwareEnabled")) {
		NetworkManager_adaptor::WimaxHardwareEnabled = static_cast<bool>(value);
	} else if (boost::iequals(property, "ActiveConnections")) {
		DBus::Path *ptr;
		int length = value.reader().get_array(&ptr);

		std::vector<DBus::Path> conns;
		for (; length >= 0; length--, ptr++) {
			conns.push_back(*ptr);
		}

		NetworkManager_adaptor::ActiveConnections = conns;
	} else if (boost::iequals(property, "Version")) {
		NetworkManager_adaptor::Version = value.reader().get_string();
	} else if (boost::iequals(property, "State")) {
		NetworkManager_adaptor::State = static_cast<uint32_t>(value);
	//} else {
	//	// fail
	}

	std::map<std::string, DBus::Variant> props;
	props[property] = value;
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

		state(NM_STATE_DISCONNECTED);
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

		std::map<DBus::Path, std::unique_ptr<Device>>::iterator it = _device_map.begin();
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
	const Connection::settings_map& connection,
	const ::DBus::Path& device,
	const ::DBus::Path& specific_object,
	::DBus::Path& path,
	::DBus::Path& active_connection)
{
	log_(0, "Adding and activating connection");

/*  // debug info
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

	// TODO check if device exists
	Device *d = _device_map[device].get();
	switch (d->DeviceType()) {
		case Device::NM_DEVICE_TYPE_ETHERNET:
			d->Enable();
			break;
		case Device::NM_DEVICE_TYPE_MODEM:
			break;
		case Device::NM_DEVICE_TYPE_WIFI:
		{
			AccessPoint::bss_id bid = static_cast<DeviceWireless*>(d)->get_access_point(specific_object);
			// TODO connect to given bssid
			//log_(0, "got ssid: ", bid.ssid, " mac: ", bid.addr.address());
		}
		break;

		case Device::NM_DEVICE_TYPE_WIMAX:
			break;
		default:
			log_(0, "Unsupported device type");
	}

	// TODO output/result parameters
	path = "/";
	active_connection = "/";
}

::DBus::Path NetworkManager::ActivateConnection(
	const ::DBus::Path& connection,
	const ::DBus::Path& device,
	const ::DBus::Path& specific_object)
{
	log_(0, "Activating connection");

	::DBus::Path r;
	// TODO
	return r;
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
	path << _dbus_path << "/Devices/" << boost::algorithm::replace_all_copy(address.address(), ":", "");

	std::unique_ptr<Device> d(
		new DeviceWireless(_connection, path.str().c_str(), _mih_user, lti));

	// wireless network hardware is now enabled
	if (!NetworkManager_adaptor::WirelessHardwareEnabled()) {
		property("WirelessHardwareEnabled", to_variant<bool>(true));
	}

	// if networking is disabled, shut this interface
	if (!NetworkManager_adaptor::NetworkingEnabled() || !NetworkManager_adaptor::WirelessEnabled()) {
		d->Disable();
	} else {
		// TODO
		// attempt to connect? if disconnected?
	}

	// save the device
	_device_map[DBus::Path(path.str())] = std::move(d);

	// signal new device
	NetworkManager_adaptor::DeviceAdded(path.str().c_str());
	log_(0, "Device added");
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

	std::unique_ptr<Device> d(
		new DeviceWired(_connection, path.str().c_str(), _mih_user, lti));

	// if networking is disabled, shut this interface
	if (!NetworkManager_adaptor::NetworkingEnabled()) {
		d->Disable();
	} else {
		// TODO
		// attempt to connect? if disconnected?
	}

	// save the device
	_device_map[DBus::Path(path.str())] = std::move(d);

	// signal new device
	NetworkManager_adaptor::DeviceAdded(path.str().c_str());
	log_(0, "Device added");
}

void NetworkManager::link_up(const mih::mac_addr &dev, const boost::optional<mih::mac_addr> &poa)
{
	log_(0, "New L2 completed for device ", dev.address(), " with poa ", poa ? poa.get().address() : "[none]");

	if (State() != NM_STATE_CONNECTED_GLOBAL ||
	    State() != NM_STATE_CONNECTED_LOCAL ||
	    State() != NM_STATE_CONNECTED_SITE) {
		state(NM_STATE_CONNECTING);
	}

	// look for device and inform/check
	bool match;
	auto it = _device_map.begin();
	while (it != _device_map.end() && !match) {
		switch (it->second.get()->DeviceType()) {
			case Device::NM_DEVICE_TYPE_WIFI:
			{
				DeviceWireless *d = reinterpret_cast<DeviceWireless *>(it->second.get());
				if (d->HwAddress() == dev.address()) {
					match = true;
					d->link_up(poa);
				}
			}
			break;

			case Device::NM_DEVICE_TYPE_ETHERNET:
			{
				DeviceWired *d = reinterpret_cast<DeviceWired *>(it->second.get());
				if (d->HwAddress() == dev.address()) {
					match = true;
					d->link_up(poa);
				}
			}
			break;

			case Device::NM_DEVICE_TYPE_WIMAX:
			{
				DeviceWiMax *d = reinterpret_cast<DeviceWiMax *>(it->second.get());
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
		switch (it->second.get()->DeviceType()) {
			case Device::NM_DEVICE_TYPE_WIFI:
			{
				DeviceWireless *d = reinterpret_cast<DeviceWireless *>(it->second.get());
				if (d->HwAddress() == dev.address()) {
					match = true;
					d->link_down();
				}
			}
			break;

			case Device::NM_DEVICE_TYPE_ETHERNET:
			{
				DeviceWired *d = reinterpret_cast<DeviceWired *>(it->second.get());
				if (d->HwAddress() == dev.address()) {
					match = true;
					d->link_down();
				}
			}
			break;

			case Device::NM_DEVICE_TYPE_WIMAX:
			{
				DeviceWiMax *d = reinterpret_cast<DeviceWiMax *>(it->second.get());
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
					it->second.get()->Disable();//Disconnect();
				} else {
					it->second.get()->Enable();
				}
			}
			it ++;
		}
	} else if (boost::iequals(prop, "WwanEnabled")) {
		// Unsupported
	} else if (boost::iequals(prop, "WimaxEnabled")) {
		// Unsupported
	}

	property(prop, value);

	PropertiesAdaptor::on_set_property(interface, prop, value);
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
					DeviceWireless *d = reinterpret_cast<DeviceWireless *>(it->second.get());
					if (boost::iequals(d->HwAddress(), dev_addr.address())) {
						d->add_ap(ldi);

						// TODO make this configurable
						// clean older scan results
						d->remove_aps_older_than(boost::posix_time::seconds(30));
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
		// TODO: notify networkmanager
		log_(0, "Received a link_going_down event");
		break;

	case mih::indication::link_handover_imminent:
		log_(0, "Received a link_handover_imminent event");
		break;

	case mih::indication::link_handover_complete:
		log_(0, "Received a link_handover_complete event");
		break;

	default:
		log_(0, "Received unknown/unsupported event");
	}
}
