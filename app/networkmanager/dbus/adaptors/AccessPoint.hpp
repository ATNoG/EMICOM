//==============================================================================
// Brief   : NetworkManager AccessPoint D-Bus interface adaptor
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

#ifndef __dbusxx_____adaptors_AccessPoint_hpp__ADAPTOR_MARSHAL_H
#define __dbusxx_____adaptors_AccessPoint_hpp__ADAPTOR_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace org {
namespace freedesktop {
namespace NetworkManager {

class AccessPoint_adaptor
: public ::DBus::InterfaceAdaptor
{
public:

	AccessPoint_adaptor()
	: ::DBus::InterfaceAdaptor("org.freedesktop.NetworkManager.AccessPoint")
	{
		bind_property(Flags, "u", true, false);
		bind_property(WpaFlags, "u", true, false);
		bind_property(RsnFlags, "u", true, false);
		bind_property(Ssid, "ay", true, false);
		bind_property(Frequency, "u", true, false);
		bind_property(HwAddress, "s", true, false);
		bind_property(Mode, "u", true, false);
		bind_property(MaxBitrate, "u", true, false);
		bind_property(Strength, "y", true, false);
	}

	::DBus::IntrospectedInterface *introspect() const 
	{
		static ::DBus::IntrospectedArgument PropertiesChanged_args[] = 
		{
			{ "properties", "a{sv}", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedMethod AccessPoint_adaptor_methods[] = 
		{
			{ 0, 0 }
		};
		static ::DBus::IntrospectedMethod AccessPoint_adaptor_signals[] = 
		{
			{ "PropertiesChanged", PropertiesChanged_args },
			{ 0, 0 }
		};
		static ::DBus::IntrospectedProperty AccessPoint_adaptor_properties[] = 
		{
			{ "Flags", "u", true, false },
			{ "WpaFlags", "u", true, false },
			{ "RsnFlags", "u", true, false },
			{ "Ssid", "ay", true, false },
			{ "Frequency", "u", true, false },
			{ "HwAddress", "s", true, false },
			{ "Mode", "u", true, false },
			{ "MaxBitrate", "u", true, false },
			{ "Strength", "y", true, false },
			{ 0, 0, 0, 0 }
		};
		static ::DBus::IntrospectedInterface AccessPoint_adaptor_interface = 
		{
			"org.freedesktop.NetworkManager.AccessPoint",
			AccessPoint_adaptor_methods,
			AccessPoint_adaptor_signals,
			AccessPoint_adaptor_properties
		};
		return &AccessPoint_adaptor_interface;
	}

public:

	/* properties exposed by this interface, use
	 * property() and property(value) to get and set a particular property
	 */
	::DBus::PropertyAdaptor< uint32_t > Flags;
	::DBus::PropertyAdaptor< uint32_t > WpaFlags;
	::DBus::PropertyAdaptor< uint32_t > RsnFlags;
	::DBus::PropertyAdaptor< std::vector< uint8_t > > Ssid;
	::DBus::PropertyAdaptor< uint32_t > Frequency;
	::DBus::PropertyAdaptor< std::string > HwAddress;
	::DBus::PropertyAdaptor< uint32_t > Mode;
	::DBus::PropertyAdaptor< uint32_t > MaxBitrate;
	::DBus::PropertyAdaptor< uint8_t > Strength;

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
#endif //__dbusxx_____adaptors_AccessPoint_hpp__ADAPTOR_MARSHAL_H
