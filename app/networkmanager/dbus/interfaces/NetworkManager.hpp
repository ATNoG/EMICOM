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

#ifndef NETWORKMANAGER__HPP_
#define NETWORKMANAGER__HPP_

#include "../adaptors/NetworkManager.hpp"

namespace odtone {
namespace networkmanager {
namespace dbus {

class NetworkManager :
	public org::freedesktop::NetworkManager_adaptor,
	public DBus::IntrospectableAdaptor,
	public DBus::PropertiesAdaptor,
	public DBus::ObjectAdaptor
{
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
};

}; }; };

#endif /* NETWORKMANAGER__HPP_ */
