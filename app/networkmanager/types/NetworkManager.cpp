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

#include <boost/algorithm/string.hpp>

using namespace odtone::networkmanager;

NetworkManager::NetworkManager(DBus::Connection &connection, const mih::config &cfg, boost::asio::io_service &io) :
	DBus::ObjectAdaptor(connection, cfg.get<std::string>(kConf_DBus_Path).c_str()),
	_connection(connection),
	_dbus_path(cfg.get<std::string>(kConf_DBus_Path)),
	_settings_path(cfg.get<std::string>(kConf_Settings_Path)),
	_settings(_connection, std::string(_dbus_path).append("/Settings").c_str(), _settings_path.c_str()),
	log_(_dbus_path.c_str(), std::cout),
	_mih_user(cfg, io,
	          boost::bind(&NetworkManager::event_handler, this, _1, _2),
	          boost::bind(&NetworkManager::new_device, this, _1, _2))
{
	// FIXME
	State = NM_STATE_DISCONNECTED;//NM_STATE_UNKNOWN;
	Version = cfg.get<std::string>(kConf_Version);
	ActiveConnections = std::vector< ::DBus::Path >();
	WimaxHardwareEnabled = false;
	WimaxEnabled = false;
	WwanHardwareEnabled = false;
	WwanEnabled = false;
	WirelessHardwareEnabled = false;
	WirelessEnabled = false;
	NetworkingEnabled = true;
}

NetworkManager::~NetworkManager()
{
}

uint32_t NetworkManager::state()
{
	return State();
}

void NetworkManager::state(NM_STATE newstate)
{
	// store
	State = newstate;

	// signal
	StateChanged(newstate);
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
		NetworkingEnabled = static_cast<bool>(value);
	} else if (boost::iequals(property, "WirelessEnabled")) {
		WirelessEnabled = static_cast<bool>(value);
	} else if (boost::iequals(property, "WirelessHardwareEnabled")) {
		WirelessHardwareEnabled = static_cast<bool>(value);
	} else if (boost::iequals(property, "WwanEnabled")) {
		WwanEnabled = static_cast<bool>(value);
	} else if (boost::iequals(property, "WwanHardwareEnabled")) {
		WwanHardwareEnabled = static_cast<bool>(value);
	} else if (boost::iequals(property, "WimaxEnabled")) {
		WimaxEnabled = static_cast<bool>(value);
	} else if (boost::iequals(property, "WimaxHardwareEnabled")) {
		WimaxHardwareEnabled = static_cast<bool>(value);
	} else if (boost::iequals(property, "ActiveConnections")) {
		DBus::Path *ptr;
		int length = value.reader().get_array(&ptr);

		std::vector<DBus::Path> conns;
		for (; length >= 0; length--, ptr++) {
			conns.push_back(*ptr);
		}

		ActiveConnections = conns;
	} else if (boost::iequals(property, "Version")) {
		Version = value.reader().get_string();
	} else if (boost::iequals(property, "State")) {
		State = static_cast<uint32_t>(value);
	//} else {
	//	// fail
	}

	std::map<std::string, DBus::Variant> props;
	props[property] = value;
	PropertiesChanged(props);
}

void NetworkManager::SetLogging(const std::string& level, const std::string& domains)
{
	// TODO
	// never?
}

std::map< std::string, std::string > NetworkManager::GetPermissions()
{
	std::map< std::string, std::string > r;
	// TODO
	// never?
	return r;
}

void NetworkManager::Enable(const bool& enable)
{
	NetworkingEnabled = enable;

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
	// TODO
}

void NetworkManager::AddAndActivateConnection(
	const std::map< std::string, std::map< std::string, ::DBus::Variant > >& connection,
	const ::DBus::Path& device,
	const ::DBus::Path& specific_object,
	::DBus::Path& path, ::DBus::Path& active_connection)
{
	// TODO
}

::DBus::Path NetworkManager::ActivateConnection(
	const ::DBus::Path& connection,
	const ::DBus::Path& device,
	const ::DBus::Path& specific_object)
{
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

	try {
		mih::link_type ltype = boost::get<mih::link_type>(type.link);
		if (ltype == mih::link_type_802_11) {
			mih::mac_addr mac = boost::get<mih::mac_addr>(address);

			add_802_11_device(mac);
		} else {
			log_(0, "Unsupported device type");
		}
	} catch (...) {
		log_(0, "MIH error adding device");
	}
}

void NetworkManager::add_802_11_device(mih::mac_addr &address)
{
	log_(0, "Adding WiFi device, address: ", address.address());

	try {
		// this is just to get the deviceindex, for a nicer D-Bus Device path
		if_80211 fi(address);

		std::stringstream path;
		path << _dbus_path << "/Devices/" << fi.ifindex();
		std::unique_ptr<Device> d(new DeviceWireless(_connection, path.str().c_str(), address));

		// wireless network hardware is now enabled
		if (!WirelessHardwareEnabled()) {
			property("WirelessHardwareEnabled", to_variant<bool>(true));
		}

		// if networking is disabled, shut this interface
		if (!NetworkingEnabled() || !WirelessEnabled()) {
			d->Disconnect();
		} else {
			// TODO
			// attempt to connect? if disconnected?
		}

		// save the device
		_device_map[DBus::Path(path.str())] = std::move(d);

		// signal new device
		DeviceAdded(path.str().c_str());
		log_(0, "Device added");
	} catch (...) {
		log_(0, "Error adding device");
	}
}

void NetworkManager::new_accesspoints_detected()
{
	log_(0, "Updating AP list on DeviceWireless devices");

	// tell DeviceWireless devices to update their list
	auto it = _device_map.begin();
	while (it != _device_map.end()) {
		if (it->second->DeviceType() == Device::NM_DEVICE_TYPE_WIFI) {
			reinterpret_cast<DeviceWireless *>(it->second.get())->refresh_accesspoint_list();
		}
		it++;
	}
}

void NetworkManager::link_up(const mih::mac_addr &dev, const mih::mac_addr &poa)
{
	log_(0, "New L2 completed for device ", dev.address(), " with poa ", poa.address());

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
                                     const std::string &property,
                                     const DBus::Variant &value)
{
	log_(0, "Setting property \"", property, "\"");

	if (boost::iequals(property, "WirelessEnabled")) {
		auto it = _device_map.begin();
		while (it != _device_map.end()) {
			if (it->second->DeviceType() == Device::NM_DEVICE_TYPE_WIFI) {
				if (!value) {
					it->second.get()->Disconnect();
				} else {
					it->second.get()->Enable();
				}
			}
			it ++;
		}
	} else if (boost::iequals(property, "WwanEnabled")) {
		// Unsupported
	} else if (boost::iequals(property, "WimaxEnabled")) {
		// Unsupported
	}

	NetworkManager::property(property, value);

	PropertiesAdaptor::on_set_property(interface, property, value);
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

		mih::mac_addr *poa;
		mih::link_addr *_poa = boost::get<mih::link_addr>(&up_link.poa_addr);
		if (_poa) {
			poa = boost::get<mih::mac_addr>(_poa);
			if (!poa) {
				log_(0, "Unable to determine poa address");
				break;
			}
		} else {
			log_(0, "Unable to determine poa address");
			break;
		}

		link_up(*dev, *poa);
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
		log_(0, "Received a link_detected event");
		new_accesspoints_detected();
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
