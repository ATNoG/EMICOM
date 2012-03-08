//==============================================================================
// Brief   : NetworkManager Device.Wired D-Bus interface adaptor
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

#ifndef __dbusxx_____adaptors_DeviceWired_hpp__ADAPTOR_MARSHAL_H
#define __dbusxx_____adaptors_DeviceWired_hpp__ADAPTOR_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace org {
namespace freedesktop {
namespace NetworkManager {
namespace Device {

class Wired_adaptor
: public ::DBus::InterfaceAdaptor
{
public:

	Wired_adaptor()
	: ::DBus::InterfaceAdaptor("org.freedesktop.NetworkManager.Device.Wired")
	{
		bind_property(HwAddress, "s", true, false);
		bind_property(PermHwAddress, "s", true, false);
		bind_property(Speed, "u", true, false);
		bind_property(Carrier, "b", true, false);
	}

	::DBus::IntrospectedInterface *introspect() const 
	{
		static ::DBus::IntrospectedArgument PropertiesChanged_args[] = 
		{
			{ "properties", "a{sv}", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedMethod Wired_adaptor_methods[] = 
		{
			{ 0, 0 }
		};
		static ::DBus::IntrospectedMethod Wired_adaptor_signals[] = 
		{
			{ "PropertiesChanged", PropertiesChanged_args },
			{ 0, 0 }
		};
		static ::DBus::IntrospectedProperty Wired_adaptor_properties[] = 
		{
			{ "HwAddress", "s", true, false },
			{ "PermHwAddress", "s", true, false },
			{ "Speed", "u", true, false },
			{ "Carrier", "b", true, false },
			{ 0, 0, 0, 0 }
		};
		static ::DBus::IntrospectedInterface Wired_adaptor_interface = 
		{
			"org.freedesktop.NetworkManager.Device.Wired",
			Wired_adaptor_methods,
			Wired_adaptor_signals,
			Wired_adaptor_properties
		};
		return &Wired_adaptor_interface;
	}

public:

	/* properties exposed by this interface, use
	 * property() and property(value) to get and set a particular property
	 */
	::DBus::PropertyAdaptor< std::string > HwAddress;
	::DBus::PropertyAdaptor< std::string > PermHwAddress;
	::DBus::PropertyAdaptor< uint32_t > Speed;
	::DBus::PropertyAdaptor< bool > Carrier;

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
#endif //__dbusxx_____adaptors_DeviceWired_hpp__ADAPTOR_MARSHAL_H
