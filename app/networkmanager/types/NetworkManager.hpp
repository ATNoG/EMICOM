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
	static const char* const NAME;
	static const char* const PATH;

	NetworkManager(DBus::Connection &connection);
	~NetworkManager();

	uint32_t state();

	void SetLogging(const std::string& level, const std::string& domains);

	std::map< std::string, std::string > GetPermissions();

	void Enable(const bool& enable);

	void Sleep(const bool& sleep);

	void DeactivateConnection(const ::DBus::Path& active_connection);

	void AddAndActivateConnection(const std::map< std::string, std::map< std::string, ::DBus::Variant > >& connection,
	                              const ::DBus::Path& device,
	                              const ::DBus::Path& specific_object,
	                              ::DBus::Path& path, ::DBus::Path& active_connection);

	::DBus::Path ActivateConnection(const ::DBus::Path& connection,
	                                const ::DBus::Path& device,
	                                const ::DBus::Path& specific_object);

	::DBus::Path GetDeviceByIpIface(const std::string& iface);

	std::vector< ::DBus::Path > GetDevices();

	void add_802_11_device(odtone::mih::mac_addr &address);

private:
	void state(NM_STATE newstate);

private:
	std::vector<std::unique_ptr<Device>> _device_list;
	DBus::Connection                    &_connection;
	odtone::logger                       log_;
};

}; };

#endif /* NETWORKMANAGER_NETWORKMANAGER__HPP_ */
