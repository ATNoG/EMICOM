//==============================================================================
// Brief   : NetworkManager DHCP4Config D-Bus interface adaptor
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

#ifndef __dbusxx_____adaptors_DHCP4Config_hpp__ADAPTOR_MARSHAL_H
#define __dbusxx_____adaptors_DHCP4Config_hpp__ADAPTOR_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace org {
namespace freedesktop {
namespace NetworkManager {

class DHCP4Config_adaptor
: public ::DBus::InterfaceAdaptor
{
public:

	DHCP4Config_adaptor()
	: ::DBus::InterfaceAdaptor("org.freedesktop.NetworkManager.DHCP4Config")
	{
		bind_property(Options, "a{sv}", true, false);
	}

	::DBus::IntrospectedInterface *introspect() const 
	{
		static ::DBus::IntrospectedArgument PropertiesChanged_args[] = 
		{
			{ "properties", "a{sv}", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedMethod DHCP4Config_adaptor_methods[] = 
		{
			{ 0, 0 }
		};
		static ::DBus::IntrospectedMethod DHCP4Config_adaptor_signals[] = 
		{
			{ "PropertiesChanged", PropertiesChanged_args },
			{ 0, 0 }
		};
		static ::DBus::IntrospectedProperty DHCP4Config_adaptor_properties[] = 
		{
			{ "Options", "a{sv}", true, false },
			{ 0, 0, 0, 0 }
		};
		static ::DBus::IntrospectedInterface DHCP4Config_adaptor_interface = 
		{
			"org.freedesktop.NetworkManager.DHCP4Config",
			DHCP4Config_adaptor_methods,
			DHCP4Config_adaptor_signals,
			DHCP4Config_adaptor_properties
		};
		return &DHCP4Config_adaptor_interface;
	}

public:

	/* properties exposed by this interface, use
	 * property() and property(value) to get and set a particular property
	 */
	::DBus::PropertyAdaptor< std::map< std::string, ::DBus::Variant > > Options;

public:

	/* methods exported by this interface,
	 * you will have to implement them in your ObjectAdaptor
	 */

public:

	/* signal emitters for this interface
	 */
	void PropertiesChanged(const std::map< std::string, ::DBus::Variant >& arg1)
	{
		::DBus::SignalMessage sig("PropertiesChanged");
		::DBus::MessageIter wi = sig.writer();
		wi << arg1;
		emit_signal(sig);
	}

private:

	/* unmarshalers (to unpack the DBus message before calling the actual interface method)
	 */
};

} } } 
#endif //__dbusxx_____adaptors_DHCP4Config_hpp__ADAPTOR_MARSHAL_H
