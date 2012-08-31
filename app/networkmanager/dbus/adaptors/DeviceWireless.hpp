//==============================================================================
// Brief   : NetworkManager Device.Wireless D-Bus interface adaptor
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

#ifndef __dbusxx_____adaptors_DeviceWireless_hpp__ADAPTOR_MARSHAL_H
#define __dbusxx_____adaptors_DeviceWireless_hpp__ADAPTOR_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace org {
namespace freedesktop {
namespace NetworkManager {
namespace Device {

class Wireless_adaptor
: public ::DBus::InterfaceAdaptor
{
public:

	Wireless_adaptor()
	: ::DBus::InterfaceAdaptor("org.freedesktop.NetworkManager.Device.Wireless")
	{
		bind_property(HwAddress, "s", true, false);
		bind_property(PermHwAddress, "s", true, false);
		bind_property(Mode, "u", true, false);
		bind_property(Bitrate, "u", true, false);
		bind_property(ActiveAccessPoint, "o", true, false);
		bind_property(WirelessCapabilities, "u", true, false);
		register_method(Wireless_adaptor, GetAccessPoints, _GetAccessPoints_stub);
		register_method(Wireless_adaptor, RequestScan, _RequestScan_stub);
	}

	::DBus::IntrospectedInterface *introspect() const
	{
		static ::DBus::IntrospectedArgument GetAccessPoints_args[] =
		{
			{ "access_points", "ao", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument RequestScan_args[] =
		{
			{ "options", "a{sv}", true },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument PropertiesChanged_args[] =
		{
			{ "properties", "a{sv}", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument AccessPointAdded_args[] =
		{
			{ "access_point", "o", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument AccessPointRemoved_args[] =
		{
			{ "access_point", "o", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedMethod Wireless_adaptor_methods[] =
		{
			{ "GetAccessPoints", GetAccessPoints_args },
			{ "RequestScan", RequestScan_args },
			{ 0, 0 }
		};
		static ::DBus::IntrospectedMethod Wireless_adaptor_signals[] =
		{
			{ "PropertiesChanged", PropertiesChanged_args },
			{ "AccessPointAdded", AccessPointAdded_args },
			{ "AccessPointRemoved", AccessPointRemoved_args },
			{ 0, 0 }
		};
		static ::DBus::IntrospectedProperty Wireless_adaptor_properties[] =
		{
			{ "HwAddress", "s", true, false },
			{ "PermHwAddress", "s", true, false },
			{ "Mode", "u", true, false },
			{ "Bitrate", "u", true, false },
			{ "ActiveAccessPoint", "o", true, false },
			{ "WirelessCapabilities", "u", true, false },
			{ 0, 0, 0, 0 }
		};
		static ::DBus::IntrospectedInterface Wireless_adaptor_interface =
		{
			"org.freedesktop.NetworkManager.Device.Wireless",
			Wireless_adaptor_methods,
			Wireless_adaptor_signals,
			Wireless_adaptor_properties
		};
		return &Wireless_adaptor_interface;
	}

public:

	/* properties exposed by this interface, use
	 * property() and property(value) to get and set a particular property
	 */
	::DBus::PropertyAdaptor< std::string > HwAddress;
	::DBus::PropertyAdaptor< std::string > PermHwAddress;
	::DBus::PropertyAdaptor< uint32_t > Mode;
	::DBus::PropertyAdaptor< uint32_t > Bitrate;
	::DBus::PropertyAdaptor< ::DBus::Path > ActiveAccessPoint;
	::DBus::PropertyAdaptor< uint32_t > WirelessCapabilities;

public:

	/* methods exported by this interface,
	 * you will have to implement them in your ObjectAdaptor
	 */
	virtual std::vector< ::DBus::Path > GetAccessPoints() = 0;
	virtual void RequestScan(const std::map< std::string, ::DBus::Variant >& options) = 0;

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
	void AccessPointAdded(const ::DBus::Path& arg1)
	{
		::DBus::SignalMessage sig("AccessPointAdded");
		::DBus::MessageIter wi = sig.writer();
		wi << arg1;
		emit_signal(sig);
	}
	void AccessPointRemoved(const ::DBus::Path& arg1)
	{
		::DBus::SignalMessage sig("AccessPointRemoved");
		::DBus::MessageIter wi = sig.writer();
		wi << arg1;
		emit_signal(sig);
	}

private:

	/* unmarshalers (to unpack the DBus message before calling the actual interface method)
	 */
	::DBus::Message _GetAccessPoints_stub(const ::DBus::CallMessage &call)
	{
		//::DBus::MessageIter ri = call.reader();

		std::vector< ::DBus::Path > argout1 = GetAccessPoints();
		::DBus::ReturnMessage reply(call);
		::DBus::MessageIter wi = reply.writer();
		wi << argout1;
		return reply;
	}
	::DBus::Message _RequestScan_stub(const ::DBus::CallMessage &call)
	{
		::DBus::MessageIter ri = call.reader();

		std::map< std::string, ::DBus::Variant > argin1; ri >> argin1;
		RequestScan(argin1);
		::DBus::ReturnMessage reply(call);
		return reply;
	}
};

} } } }
#endif //__dbusxx_____adaptors_DeviceWireless_hpp__ADAPTOR_MARSHAL_H
