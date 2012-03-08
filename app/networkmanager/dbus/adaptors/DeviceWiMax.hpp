//==============================================================================
// Brief   : NetworkManager Device.WiMax D-Bus interface adaptor
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

#ifndef __dbusxx_____adaptors_DeviceWiMax_hpp__ADAPTOR_MARSHAL_H
#define __dbusxx_____adaptors_DeviceWiMax_hpp__ADAPTOR_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace org {
namespace freedesktop {
namespace NetworkManager {
namespace Device {

class WiMax_adaptor
: public ::DBus::InterfaceAdaptor
{
public:

	WiMax_adaptor()
	: ::DBus::InterfaceAdaptor("org.freedesktop.NetworkManager.Device.WiMax")
	{
		bind_property(HwAddress, "s", true, false);
		bind_property(CenterFrequency, "u", true, false);
		bind_property(Rssi, "i", true, false);
		bind_property(Cinr, "i", true, false);
		bind_property(TxPower, "i", true, false);
		bind_property(Bsid, "s", true, false);
		bind_property(ActiveNsp, "o", true, false);
		register_method(WiMax_adaptor, GetNspList, _GetNspList_stub);
	}

	::DBus::IntrospectedInterface *introspect() const 
	{
		static ::DBus::IntrospectedArgument GetNspList_args[] = 
		{
			{ "nsps", "ao", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument PropertiesChanged_args[] = 
		{
			{ "properties", "a{sv}", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument NspAdded_args[] = 
		{
			{ "nsp", "o", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument NspRemoved_args[] = 
		{
			{ "nsp", "o", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedMethod WiMax_adaptor_methods[] = 
		{
			{ "GetNspList", GetNspList_args },
			{ 0, 0 }
		};
		static ::DBus::IntrospectedMethod WiMax_adaptor_signals[] = 
		{
			{ "PropertiesChanged", PropertiesChanged_args },
			{ "NspAdded", NspAdded_args },
			{ "NspRemoved", NspRemoved_args },
			{ 0, 0 }
		};
		static ::DBus::IntrospectedProperty WiMax_adaptor_properties[] = 
		{
			{ "HwAddress", "s", true, false },
			{ "CenterFrequency", "u", true, false },
			{ "Rssi", "i", true, false },
			{ "Cinr", "i", true, false },
			{ "TxPower", "i", true, false },
			{ "Bsid", "s", true, false },
			{ "ActiveNsp", "o", true, false },
			{ 0, 0, 0, 0 }
		};
		static ::DBus::IntrospectedInterface WiMax_adaptor_interface = 
		{
			"org.freedesktop.NetworkManager.Device.WiMax",
			WiMax_adaptor_methods,
			WiMax_adaptor_signals,
			WiMax_adaptor_properties
		};
		return &WiMax_adaptor_interface;
	}

public:

	/* properties exposed by this interface, use
	 * property() and property(value) to get and set a particular property
	 */
	::DBus::PropertyAdaptor< std::string > HwAddress;
	::DBus::PropertyAdaptor< uint32_t > CenterFrequency;
	::DBus::PropertyAdaptor< int32_t > Rssi;
	::DBus::PropertyAdaptor< int32_t > Cinr;
	::DBus::PropertyAdaptor< int32_t > TxPower;
	::DBus::PropertyAdaptor< std::string > Bsid;
	::DBus::PropertyAdaptor< ::DBus::Path > ActiveNsp;

public:

	/* methods exported by this interface,
	 * you will have to implement them in your ObjectAdaptor
	 */
	virtual std::vector< ::DBus::Path > GetNspList() = 0;

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
	void NspAdded(const ::DBus::Path& arg1)
	{
		::DBus::SignalMessage sig("NspAdded");
		::DBus::MessageIter wi = sig.writer();
		wi << arg1;
		emit_signal(sig);
	}
	void NspRemoved(const ::DBus::Path& arg1)
	{
		::DBus::SignalMessage sig("NspRemoved");
		::DBus::MessageIter wi = sig.writer();
		wi << arg1;
		emit_signal(sig);
	}

private:

	/* unmarshalers (to unpack the DBus message before calling the actual interface method)
	 */
	::DBus::Message _GetNspList_stub(const ::DBus::CallMessage &call)
	{
		//::DBus::MessageIter ri = call.reader();

		std::vector< ::DBus::Path > argout1 = GetNspList();
		::DBus::ReturnMessage reply(call);
		::DBus::MessageIter wi = reply.writer();
		wi << argout1;
		return reply;
	}
};

} } } } 
#endif //__dbusxx_____adaptors_DeviceWiMax_hpp__ADAPTOR_MARSHAL_H
