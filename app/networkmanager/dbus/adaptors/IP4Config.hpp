//==============================================================================
// Brief   : NetworkManager IP4Config D-Bus interface adaptor
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

#ifndef __dbusxx_____adaptors_IP4Config_hpp__ADAPTOR_MARSHAL_H
#define __dbusxx_____adaptors_IP4Config_hpp__ADAPTOR_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace org {
namespace freedesktop {
namespace NetworkManager {

class IP4Config_adaptor
: public ::DBus::InterfaceAdaptor
{
public:

	IP4Config_adaptor()
	: ::DBus::InterfaceAdaptor("org.freedesktop.NetworkManager.IP4Config")
	{
		bind_property(Addresses, "aau", true, false);
		bind_property(Nameservers, "au", true, false);
		bind_property(WinsServers, "au", true, false);
		bind_property(Domains, "as", true, false);
		bind_property(Routes, "aau", true, false);
	}

	::DBus::IntrospectedInterface *introspect() const 
	{
		static ::DBus::IntrospectedMethod IP4Config_adaptor_methods[] = 
		{
			{ 0, 0 }
		};
		static ::DBus::IntrospectedMethod IP4Config_adaptor_signals[] = 
		{
			{ 0, 0 }
		};
		static ::DBus::IntrospectedProperty IP4Config_adaptor_properties[] = 
		{
			{ "Addresses", "aau", true, false },
			{ "Nameservers", "au", true, false },
			{ "WinsServers", "au", true, false },
			{ "Domains", "as", true, false },
			{ "Routes", "aau", true, false },
			{ 0, 0, 0, 0 }
		};
		static ::DBus::IntrospectedInterface IP4Config_adaptor_interface = 
		{
			"org.freedesktop.NetworkManager.IP4Config",
			IP4Config_adaptor_methods,
			IP4Config_adaptor_signals,
			IP4Config_adaptor_properties
		};
		return &IP4Config_adaptor_interface;
	}

public:

	/* properties exposed by this interface, use
	 * property() and property(value) to get and set a particular property
	 */
	::DBus::PropertyAdaptor< std::vector< std::vector< uint32_t > > > Addresses;
	::DBus::PropertyAdaptor< std::vector< uint32_t > > Nameservers;
	::DBus::PropertyAdaptor< std::vector< uint32_t > > WinsServers;
	::DBus::PropertyAdaptor< std::vector< std::string > > Domains;
	::DBus::PropertyAdaptor< std::vector< std::vector< uint32_t > > > Routes;

public:

	/* methods exported by this interface,
	 * you will have to implement them in your ObjectAdaptor
	 */

public:

	/* signal emitters for this interface
	 */

private:

	/* unmarshalers (to unpack the DBus message before calling the actual interface method)
	 */
};

} } } 
#endif //__dbusxx_____adaptors_IP4Config_hpp__ADAPTOR_MARSHAL_H
