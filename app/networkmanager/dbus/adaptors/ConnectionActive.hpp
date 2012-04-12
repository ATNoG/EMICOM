//==============================================================================
// Brief   : NetworkManager Connection.Active D-Bus interface adaptor
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

#ifndef __dbusxx_____adaptors_ConnectionActive_hpp__ADAPTOR_MARSHAL_H
#define __dbusxx_____adaptors_ConnectionActive_hpp__ADAPTOR_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace org {
namespace freedesktop {
namespace NetworkManager {
namespace Connection {

class Active_adaptor
: public ::DBus::InterfaceAdaptor
{
public:

	Active_adaptor()
	: ::DBus::InterfaceAdaptor("org.freedesktop.NetworkManager.Connection.Active")
	{
		bind_property(Connection, "o", true, false);
		bind_property(SpecificObject, "o", true, false);
		bind_property(Uuid, "s", true, false);
		bind_property(Devices, "ao", true, false);
		bind_property(State, "u", true, false);
		bind_property(Default, "b", true, false);
		bind_property(Default6, "b", true, false);
		bind_property(Vpn, "b", true, false);
		bind_property(Master, "o", true, false);
	}

	::DBus::IntrospectedInterface *introspect() const 
	{
		static ::DBus::IntrospectedArgument PropertiesChanged_args[] = 
		{
			{ "properties", "a{sv}", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedMethod Active_adaptor_methods[] = 
		{
			{ 0, 0 }
		};
		static ::DBus::IntrospectedMethod Active_adaptor_signals[] = 
		{
			{ "PropertiesChanged", PropertiesChanged_args },
			{ 0, 0 }
		};
		static ::DBus::IntrospectedProperty Active_adaptor_properties[] = 
		{
			{ "Connection", "o", true, false },
			{ "SpecificObject", "o", true, false },
			{ "Uuid", "s", true, false },
			{ "Devices", "ao", true, false },
			{ "State", "u", true, false },
			{ "Default", "b", true, false },
			{ "Default6", "b", true, false },
			{ "Vpn", "b", true, false },
			{ "Master", "o", true, false },
			{ 0, 0, 0, 0 }
		};
		static ::DBus::IntrospectedInterface Active_adaptor_interface = 
		{
			"org.freedesktop.NetworkManager.Connection.Active",
			Active_adaptor_methods,
			Active_adaptor_signals,
			Active_adaptor_properties
		};
		return &Active_adaptor_interface;
	}

public:

	/* properties exposed by this interface, use
	 * property() and property(value) to get and set a particular property
	 */
	::DBus::PropertyAdaptor< ::DBus::Path > Connection;
	::DBus::PropertyAdaptor< ::DBus::Path > SpecificObject;
	::DBus::PropertyAdaptor< std::string > Uuid;
	::DBus::PropertyAdaptor< std::vector< ::DBus::Path > > Devices;
	::DBus::PropertyAdaptor< uint32_t > State;
	::DBus::PropertyAdaptor< bool > Default;
	::DBus::PropertyAdaptor< bool > Default6;
	::DBus::PropertyAdaptor< bool > Vpn;
	::DBus::PropertyAdaptor< ::DBus::Path > Master;

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

} } } } 
#endif //__dbusxx_____adaptors_ConnectionActive_hpp__ADAPTOR_MARSHAL_H
