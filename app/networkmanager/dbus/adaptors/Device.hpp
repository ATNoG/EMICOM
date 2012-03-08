//==============================================================================
// Brief   : NetworkManager Device D-Bus interface adaptor
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

#ifndef __dbusxx_____adaptors_Device_hpp__ADAPTOR_MARSHAL_H
#define __dbusxx_____adaptors_Device_hpp__ADAPTOR_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace org {
namespace freedesktop {
namespace NetworkManager {

class Device_adaptor
: public ::DBus::InterfaceAdaptor
{
public:

	Device_adaptor()
	: ::DBus::InterfaceAdaptor("org.freedesktop.NetworkManager.Device")
	{
		bind_property(Udi, "s", true, false);
		bind_property(Interface, "s", true, false);
		bind_property(IpInterface, "s", true, false);
		bind_property(Driver, "s", true, false);
		bind_property(Capabilities, "u", true, false);
		bind_property(Ip4Address, "i", true, false);
		bind_property(State, "u", true, false);
		bind_property(StateReason, "(uu)", true, false);
		bind_property(ActiveConnection, "o", true, false);
		bind_property(Ip4Config, "o", true, false);
		bind_property(Dhcp4Config, "o", true, false);
		bind_property(Ip6Config, "o", true, false);
		bind_property(Dhcp6Config, "o", true, false);
		bind_property(Managed, "b", true, false);
		bind_property(FirmwareMissing, "b", true, false);
		bind_property(DeviceType, "u", true, false);
		register_method(Device_adaptor, Disconnect, _Disconnect_stub);
	}

	::DBus::IntrospectedInterface *introspect() const 
	{
		static ::DBus::IntrospectedArgument Disconnect_args[] = 
		{
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument StateChanged_args[] = 
		{
			{ "new_state", "u", false },
			{ "old_state", "u", false },
			{ "reason", "u", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedMethod Device_adaptor_methods[] = 
		{
			{ "Disconnect", Disconnect_args },
			{ 0, 0 }
		};
		static ::DBus::IntrospectedMethod Device_adaptor_signals[] = 
		{
			{ "StateChanged", StateChanged_args },
			{ 0, 0 }
		};
		static ::DBus::IntrospectedProperty Device_adaptor_properties[] = 
		{
			{ "Udi", "s", true, false },
			{ "Interface", "s", true, false },
			{ "IpInterface", "s", true, false },
			{ "Driver", "s", true, false },
			{ "Capabilities", "u", true, false },
			{ "Ip4Address", "i", true, false },
			{ "State", "u", true, false },
			{ "StateReason", "(uu)", true, false },
			{ "ActiveConnection", "o", true, false },
			{ "Ip4Config", "o", true, false },
			{ "Dhcp4Config", "o", true, false },
			{ "Ip6Config", "o", true, false },
			{ "Dhcp6Config", "o", true, false },
			{ "Managed", "b", true, false },
			{ "FirmwareMissing", "b", true, false },
			{ "DeviceType", "u", true, false },
			{ 0, 0, 0, 0 }
		};
		static ::DBus::IntrospectedInterface Device_adaptor_interface = 
		{
			"org.freedesktop.NetworkManager.Device",
			Device_adaptor_methods,
			Device_adaptor_signals,
			Device_adaptor_properties
		};
		return &Device_adaptor_interface;
	}

public:

	/* properties exposed by this interface, use
	 * property() and property(value) to get and set a particular property
	 */
	::DBus::PropertyAdaptor< std::string > Udi;
	::DBus::PropertyAdaptor< std::string > Interface;
	::DBus::PropertyAdaptor< std::string > IpInterface;
	::DBus::PropertyAdaptor< std::string > Driver;
	::DBus::PropertyAdaptor< uint32_t > Capabilities;
	::DBus::PropertyAdaptor< int32_t > Ip4Address;
	::DBus::PropertyAdaptor< uint32_t > State;
	::DBus::PropertyAdaptor< ::DBus::Struct< uint32_t, uint32_t > > StateReason;
	::DBus::PropertyAdaptor< ::DBus::Path > ActiveConnection;
	::DBus::PropertyAdaptor< ::DBus::Path > Ip4Config;
	::DBus::PropertyAdaptor< ::DBus::Path > Dhcp4Config;
	::DBus::PropertyAdaptor< ::DBus::Path > Ip6Config;
	::DBus::PropertyAdaptor< ::DBus::Path > Dhcp6Config;
	::DBus::PropertyAdaptor< bool > Managed;
	::DBus::PropertyAdaptor< bool > FirmwareMissing;
	::DBus::PropertyAdaptor< uint32_t > DeviceType;

public:

	/* methods exported by this interface,
	 * you will have to implement them in your ObjectAdaptor
	 */
	virtual void Disconnect() = 0;

public:

	/* signal emitters for this interface
	 */
	void StateChanged(const uint32_t& arg1, const uint32_t& arg2, const uint32_t& arg3)
	{
		::DBus::SignalMessage sig("StateChanged");
		::DBus::MessageIter wi = sig.writer();
		wi << arg1;
		wi << arg2;
		wi << arg3;
		emit_signal(sig);
	}

private:

	/* unmarshalers (to unpack the DBus message before calling the actual interface method)
	 */
	::DBus::Message _Disconnect_stub(const ::DBus::CallMessage &call)
	{
		//::DBus::MessageIter ri = call.reader();

		Disconnect();
		::DBus::ReturnMessage reply(call);
		return reply;
	}
};

} } } 
#endif //__dbusxx_____adaptors_Device_hpp__ADAPTOR_MARSHAL_H
