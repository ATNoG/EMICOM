//==============================================================================
// Brief   : NetworkManager IP6Config D-Bus interface adaptor
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

#ifndef __dbusxx_____adaptors_IP6Config_hpp__ADAPTOR_MARSHAL_H
#define __dbusxx_____adaptors_IP6Config_hpp__ADAPTOR_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace org {
namespace freedesktop {
namespace NetworkManager {

class IP6Config_adaptor
: public ::DBus::InterfaceAdaptor
{
public:

	IP6Config_adaptor()
	: ::DBus::InterfaceAdaptor("org.freedesktop.NetworkManager.IP6Config")
	{
		bind_property(Addresses, "a(ayuay)", true, false);
		bind_property(Nameservers, "aay", true, false);
		bind_property(Domains, "as", true, false);
		bind_property(Routes, "a(ayuayu)", true, false);
	}

	::DBus::IntrospectedInterface *introspect() const 
	{
		static ::DBus::IntrospectedMethod IP6Config_adaptor_methods[] = 
		{
			{ 0, 0 }
		};
		static ::DBus::IntrospectedMethod IP6Config_adaptor_signals[] = 
		{
			{ 0, 0 }
		};
		static ::DBus::IntrospectedProperty IP6Config_adaptor_properties[] = 
		{
			{ "Addresses", "a(ayuay)", true, false },
			{ "Nameservers", "aay", true, false },
			{ "Domains", "as", true, false },
			{ "Routes", "a(ayuayu)", true, false },
			{ 0, 0, 0, 0 }
		};
		static ::DBus::IntrospectedInterface IP6Config_adaptor_interface = 
		{
			"org.freedesktop.NetworkManager.IP6Config",
			IP6Config_adaptor_methods,
			IP6Config_adaptor_signals,
			IP6Config_adaptor_properties
		};
		return &IP6Config_adaptor_interface;
	}

public:

	/* properties exposed by this interface, use
	 * property() and property(value) to get and set a particular property
	 */
	::DBus::PropertyAdaptor< std::vector< ::DBus::Struct< std::vector< uint8_t >, uint32_t, std::vector< uint8_t > > > > Addresses;
	::DBus::PropertyAdaptor< std::vector< std::vector< uint8_t > > > Nameservers;
	::DBus::PropertyAdaptor< std::vector< std::string > > Domains;
	::DBus::PropertyAdaptor< std::vector< ::DBus::Struct< std::vector< uint8_t >, uint32_t, std::vector< uint8_t >, uint32_t > > > Routes;

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
#endif //__dbusxx_____adaptors_IP6Config_hpp__ADAPTOR_MARSHAL_H
