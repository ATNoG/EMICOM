//==============================================================================
// Brief   : NetworkManager Device.Modem D-Bus interface adaptor
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

#ifndef __dbusxx_____adaptors_DeviceModem_hpp__ADAPTOR_MARSHAL_H
#define __dbusxx_____adaptors_DeviceModem_hpp__ADAPTOR_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace org {
namespace freedesktop {
namespace NetworkManager {
namespace Device {

class Modem_adaptor
: public ::DBus::InterfaceAdaptor
{
public:

	Modem_adaptor()
	: ::DBus::InterfaceAdaptor("org.freedesktop.NetworkManager.Device.Modem")
	{
		bind_property(ModemCapabilities, "u", true, false);
		bind_property(CurrentCapabilities, "u", true, false);
	}

	::DBus::IntrospectedInterface *introspect() const 
	{
		static ::DBus::IntrospectedArgument PropertiesChanged_args[] = 
		{
			{ "properties", "a{sv}", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedMethod Modem_adaptor_methods[] = 
		{
			{ 0, 0 }
		};
		static ::DBus::IntrospectedMethod Modem_adaptor_signals[] = 
		{
			{ "PropertiesChanged", PropertiesChanged_args },
			{ 0, 0 }
		};
		static ::DBus::IntrospectedProperty Modem_adaptor_properties[] = 
		{
			{ "ModemCapabilities", "u", true, false },
			{ "CurrentCapabilities", "u", true, false },
			{ 0, 0, 0, 0 }
		};
		static ::DBus::IntrospectedInterface Modem_adaptor_interface = 
		{
			"org.freedesktop.NetworkManager.Device.Modem",
			Modem_adaptor_methods,
			Modem_adaptor_signals,
			Modem_adaptor_properties
		};
		return &Modem_adaptor_interface;
	}

public:

	/* properties exposed by this interface, use
	 * property() and property(value) to get and set a particular property
	 */
	::DBus::PropertyAdaptor< uint32_t > ModemCapabilities;
	::DBus::PropertyAdaptor< uint32_t > CurrentCapabilities;

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
#endif //__dbusxx_____adaptors_DeviceModem_hpp__ADAPTOR_MARSHAL_H
