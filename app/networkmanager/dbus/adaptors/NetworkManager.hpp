//==============================================================================
// Brief   : NetworkManager D-Bus interface adaptor
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

#ifndef __dbusxx_____adaptors_NetworkManager_hpp__ADAPTOR_MARSHAL_H
#define __dbusxx_____adaptors_NetworkManager_hpp__ADAPTOR_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace org {
namespace freedesktop {

class NetworkManager_adaptor
: public ::DBus::InterfaceAdaptor
{
public:

	NetworkManager_adaptor()
	: ::DBus::InterfaceAdaptor("org.freedesktop.NetworkManager")
	{
		bind_property(NetworkingEnabled, "b", true, false);
		bind_property(WirelessEnabled, "b", true, true);
		bind_property(WirelessHardwareEnabled, "b", true, false);
		bind_property(WwanEnabled, "b", true, true);
		bind_property(WwanHardwareEnabled, "b", true, false);
		bind_property(WimaxEnabled, "b", true, true);
		bind_property(WimaxHardwareEnabled, "b", true, false);
		bind_property(ActiveConnections, "ao", true, false);
		bind_property(Version, "s", true, false);
		bind_property(State, "u", true, false);
		register_method(NetworkManager_adaptor, GetDevices, _GetDevices_stub);
		register_method(NetworkManager_adaptor, GetDeviceByIpIface, _GetDeviceByIpIface_stub);
		register_method(NetworkManager_adaptor, ActivateConnection, _ActivateConnection_stub);
		register_method(NetworkManager_adaptor, AddAndActivateConnection, _AddAndActivateConnection_stub);
		register_method(NetworkManager_adaptor, DeactivateConnection, _DeactivateConnection_stub);
		register_method(NetworkManager_adaptor, Sleep, _Sleep_stub);
		register_method(NetworkManager_adaptor, Enable, _Enable_stub);
		register_method(NetworkManager_adaptor, GetPermissions, _GetPermissions_stub);
		register_method(NetworkManager_adaptor, SetLogging, _SetLogging_stub);
		register_method(NetworkManager_adaptor, state, _state_stub);
	}

	::DBus::IntrospectedInterface *introspect() const 
	{
		static ::DBus::IntrospectedArgument GetDevices_args[] = 
		{
			{ "devices", "ao", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument GetDeviceByIpIface_args[] = 
		{
			{ "iface", "s", true },
			{ "device", "o", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument ActivateConnection_args[] = 
		{
			{ "connection", "o", true },
			{ "device", "o", true },
			{ "specific_object", "o", true },
			{ "active_connection", "o", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument AddAndActivateConnection_args[] = 
		{
			{ "connection", "a{sa{sv}}", true },
			{ "device", "o", true },
			{ "specific_object", "o", true },
			{ "path", "o", false },
			{ "active_connection", "o", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument DeactivateConnection_args[] = 
		{
			{ "active_connection", "o", true },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument Sleep_args[] = 
		{
			{ "sleep", "b", true },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument Enable_args[] = 
		{
			{ "enable", "b", true },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument GetPermissions_args[] = 
		{
			{ "permissions", "a{ss}", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument SetLogging_args[] = 
		{
			{ "level", "s", true },
			{ "domains", "s", true },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument state_args[] = 
		{
			{ "state", "u", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument CheckPermissions_args[] = 
		{
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument StateChanged_args[] = 
		{
			{ "state", "u", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument PropertiesChanged_args[] = 
		{
			{ "properties", "a{sv}", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument DeviceAdded_args[] = 
		{
			{ "device_path", "o", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument DeviceRemoved_args[] = 
		{
			{ "device_path", "o", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedMethod NetworkManager_adaptor_methods[] = 
		{
			{ "GetDevices", GetDevices_args },
			{ "GetDeviceByIpIface", GetDeviceByIpIface_args },
			{ "ActivateConnection", ActivateConnection_args },
			{ "AddAndActivateConnection", AddAndActivateConnection_args },
			{ "DeactivateConnection", DeactivateConnection_args },
			{ "Sleep", Sleep_args },
			{ "Enable", Enable_args },
			{ "GetPermissions", GetPermissions_args },
			{ "SetLogging", SetLogging_args },
			{ "state", state_args },
			{ 0, 0 }
		};
		static ::DBus::IntrospectedMethod NetworkManager_adaptor_signals[] = 
		{
			{ "CheckPermissions", CheckPermissions_args },
			{ "StateChanged", StateChanged_args },
			{ "PropertiesChanged", PropertiesChanged_args },
			{ "DeviceAdded", DeviceAdded_args },
			{ "DeviceRemoved", DeviceRemoved_args },
			{ 0, 0 }
		};
		static ::DBus::IntrospectedProperty NetworkManager_adaptor_properties[] = 
		{
			{ "NetworkingEnabled", "b", true, false },
			{ "WirelessEnabled", "b", true, true },
			{ "WirelessHardwareEnabled", "b", true, false },
			{ "WwanEnabled", "b", true, true },
			{ "WwanHardwareEnabled", "b", true, false },
			{ "WimaxEnabled", "b", true, true },
			{ "WimaxHardwareEnabled", "b", true, false },
			{ "ActiveConnections", "ao", true, false },
			{ "Version", "s", true, false },
			{ "State", "u", true, false },
			{ 0, 0, 0, 0 }
		};
		static ::DBus::IntrospectedInterface NetworkManager_adaptor_interface = 
		{
			"org.freedesktop.NetworkManager",
			NetworkManager_adaptor_methods,
			NetworkManager_adaptor_signals,
			NetworkManager_adaptor_properties
		};
		return &NetworkManager_adaptor_interface;
	}

public:

	/* properties exposed by this interface, use
	 * property() and property(value) to get and set a particular property
	 */
	::DBus::PropertyAdaptor< bool > NetworkingEnabled;
	::DBus::PropertyAdaptor< bool > WirelessEnabled;
	::DBus::PropertyAdaptor< bool > WirelessHardwareEnabled;
	::DBus::PropertyAdaptor< bool > WwanEnabled;
	::DBus::PropertyAdaptor< bool > WwanHardwareEnabled;
	::DBus::PropertyAdaptor< bool > WimaxEnabled;
	::DBus::PropertyAdaptor< bool > WimaxHardwareEnabled;
	::DBus::PropertyAdaptor< std::vector< ::DBus::Path > > ActiveConnections;
	::DBus::PropertyAdaptor< std::string > Version;
	::DBus::PropertyAdaptor< uint32_t > State;

public:

	/* methods exported by this interface,
	 * you will have to implement them in your ObjectAdaptor
	 */
	virtual std::vector< ::DBus::Path > GetDevices() = 0;
	virtual ::DBus::Path GetDeviceByIpIface(const std::string& iface) = 0;
	virtual ::DBus::Path ActivateConnection(const ::DBus::Path& connection, const ::DBus::Path& device, const ::DBus::Path& specific_object) = 0;
	virtual void AddAndActivateConnection(const std::map< std::string, std::map< std::string, ::DBus::Variant > >& connection, const ::DBus::Path& device, const ::DBus::Path& specific_object, ::DBus::Path& path, ::DBus::Path& active_connection) = 0;
	virtual void DeactivateConnection(const ::DBus::Path& active_connection) = 0;
	virtual void Sleep(const bool& sleep) = 0;
	virtual void Enable(const bool& enable) = 0;
	virtual std::map< std::string, std::string > GetPermissions() = 0;
	virtual void SetLogging(const std::string& level, const std::string& domains) = 0;
	virtual uint32_t state() = 0;

public:

	/* signal emitters for this interface
	 */
	void CheckPermissions()
	{
		::DBus::SignalMessage sig("CheckPermissions");
		emit_signal(sig);
	}
	void StateChanged(const uint32_t& arg1)
	{
		::DBus::SignalMessage sig("StateChanged");
		::DBus::MessageIter wi = sig.writer();
		wi << arg1;
		emit_signal(sig);
	}
	void PropertiesChanged(const std::map< std::string, ::DBus::Variant >& arg1)
	{
		::DBus::SignalMessage sig("PropertiesChanged");
		::DBus::MessageIter wi = sig.writer();
		wi << arg1;
		emit_signal(sig);
	}
	void DeviceAdded(const ::DBus::Path& arg1)
	{
		::DBus::SignalMessage sig("DeviceAdded");
		::DBus::MessageIter wi = sig.writer();
		wi << arg1;
		emit_signal(sig);
	}
	void DeviceRemoved(const ::DBus::Path& arg1)
	{
		::DBus::SignalMessage sig("DeviceRemoved");
		::DBus::MessageIter wi = sig.writer();
		wi << arg1;
		emit_signal(sig);
	}

private:

	/* unmarshalers (to unpack the DBus message before calling the actual interface method)
	 */
	::DBus::Message _GetDevices_stub(const ::DBus::CallMessage &call)
	{
		//::DBus::MessageIter ri = call.reader();

		std::vector< ::DBus::Path > argout1 = GetDevices();
		::DBus::ReturnMessage reply(call);
		::DBus::MessageIter wi = reply.writer();
		wi << argout1;
		return reply;
	}
	::DBus::Message _GetDeviceByIpIface_stub(const ::DBus::CallMessage &call)
	{
		::DBus::MessageIter ri = call.reader();

		std::string argin1; ri >> argin1;
		::DBus::Path argout1 = GetDeviceByIpIface(argin1);
		::DBus::ReturnMessage reply(call);
		::DBus::MessageIter wi = reply.writer();
		wi << argout1;
		return reply;
	}
	::DBus::Message _ActivateConnection_stub(const ::DBus::CallMessage &call)
	{
		::DBus::MessageIter ri = call.reader();

		::DBus::Path argin1; ri >> argin1;
		::DBus::Path argin2; ri >> argin2;
		::DBus::Path argin3; ri >> argin3;
		::DBus::Path argout1 = ActivateConnection(argin1, argin2, argin3);
		::DBus::ReturnMessage reply(call);
		::DBus::MessageIter wi = reply.writer();
		wi << argout1;
		return reply;
	}
	::DBus::Message _AddAndActivateConnection_stub(const ::DBus::CallMessage &call)
	{
		::DBus::MessageIter ri = call.reader();

		std::map< std::string, std::map< std::string, ::DBus::Variant > > argin1; ri >> argin1;
		::DBus::Path argin2; ri >> argin2;
		::DBus::Path argin3; ri >> argin3;
		// nm-applet uses both these parameters empty when activating a DeviceWired
		::DBus::Path argout1;
		::DBus::Path argout2;
		AddAndActivateConnection(argin1, argin2, argin3, argout1, argout2);
		::DBus::ReturnMessage reply(call);
		::DBus::MessageIter wi = reply.writer();
		wi << argout1;
		wi << argout2;
		return reply;
	}
	::DBus::Message _DeactivateConnection_stub(const ::DBus::CallMessage &call)
	{
		::DBus::MessageIter ri = call.reader();

		::DBus::Path argin1; ri >> argin1;
		DeactivateConnection(argin1);
		::DBus::ReturnMessage reply(call);
		return reply;
	}
	::DBus::Message _Sleep_stub(const ::DBus::CallMessage &call)
	{
		::DBus::MessageIter ri = call.reader();

		bool argin1; ri >> argin1;
		Sleep(argin1);
		::DBus::ReturnMessage reply(call);
		return reply;
	}
	::DBus::Message _Enable_stub(const ::DBus::CallMessage &call)
	{
		::DBus::MessageIter ri = call.reader();

		bool argin1; ri >> argin1;
		Enable(argin1);
		::DBus::ReturnMessage reply(call);
		return reply;
	}
	::DBus::Message _GetPermissions_stub(const ::DBus::CallMessage &call)
	{
		//::DBus::MessageIter ri = call.reader();

		std::map< std::string, std::string > argout1 = GetPermissions();
		::DBus::ReturnMessage reply(call);
		::DBus::MessageIter wi = reply.writer();
		wi << argout1;
		return reply;
	}
	::DBus::Message _SetLogging_stub(const ::DBus::CallMessage &call)
	{
		::DBus::MessageIter ri = call.reader();

		std::string argin1; ri >> argin1;
		std::string argin2; ri >> argin2;
		SetLogging(argin1, argin2);
		::DBus::ReturnMessage reply(call);
		return reply;
	}
	::DBus::Message _state_stub(const ::DBus::CallMessage &call)
	{
		//::DBus::MessageIter ri = call.reader();

		uint32_t argout1 = state();
		::DBus::ReturnMessage reply(call);
		::DBus::MessageIter wi = reply.writer();
		wi << argout1;
		return reply;
	}
};

} } 
#endif //__dbusxx_____adaptors_NetworkManager_hpp__ADAPTOR_MARSHAL_H
