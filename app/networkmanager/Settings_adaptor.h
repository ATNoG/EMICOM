//==============================================================================
// Brief   : NetworkManager Settings interface adaptor
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

/*
 *	This file was automatically generated by dbusxx-xml2cpp; DO NOT EDIT!
 */

#ifndef __dbusxx__Settings_adaptor_h__ADAPTOR_MARSHAL_H
#define __dbusxx__Settings_adaptor_h__ADAPTOR_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace org {
namespace freedesktop {
namespace NetworkManager {

class Settings_adaptor
: public ::DBus::InterfaceAdaptor
{
public:

	Settings_adaptor()
	: ::DBus::InterfaceAdaptor("org.freedesktop.NetworkManager.Settings")
	{
		bind_property(CanModify, "b", true, false);
		bind_property(Hostname, "s", true, false);
		register_method(Settings_adaptor, SaveHostname, _SaveHostname_stub);
		register_method(Settings_adaptor, AddConnection, _AddConnection_stub);
		register_method(Settings_adaptor, GetConnectionByUuid, _GetConnectionByUuid_stub);
		register_method(Settings_adaptor, ListConnections, _ListConnections_stub);
	}

	::DBus::IntrospectedInterface *introspect() const 
	{
		static ::DBus::IntrospectedArgument SaveHostname_args[] = 
		{
			{ "hostname", "s", true },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument AddConnection_args[] = 
		{
			{ "connection", "a{sa{sv}}", true },
			{ "path", "o", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument GetConnectionByUuid_args[] = 
		{
			{ "uuid", "s", true },
			{ "connection", "o", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument ListConnections_args[] = 
		{
			{ "connections", "ao", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument NewConnection_args[] = 
		{
			{ 0, "o", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument PropertiesChanged_args[] = 
		{
			{ 0, "a{sv}", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedMethod Settings_adaptor_methods[] = 
		{
			{ "SaveHostname", SaveHostname_args },
			{ "AddConnection", AddConnection_args },
			{ "GetConnectionByUuid", GetConnectionByUuid_args },
			{ "ListConnections", ListConnections_args },
			{ 0, 0 }
		};
		static ::DBus::IntrospectedMethod Settings_adaptor_signals[] = 
		{
			{ "NewConnection", NewConnection_args },
			{ "PropertiesChanged", PropertiesChanged_args },
			{ 0, 0 }
		};
		static ::DBus::IntrospectedProperty Settings_adaptor_properties[] = 
		{
			{ "CanModify", "b", true, false },
			{ "Hostname", "s", true, false },
			{ 0, 0, 0, 0 }
		};
		static ::DBus::IntrospectedInterface Settings_adaptor_interface = 
		{
			"org.freedesktop.NetworkManager.Settings",
			Settings_adaptor_methods,
			Settings_adaptor_signals,
			Settings_adaptor_properties
		};
		return &Settings_adaptor_interface;
	}

public:

	/* properties exposed by this interface, use
	 * property() and property(value) to get and set a particular property
	 */
	::DBus::PropertyAdaptor< bool > CanModify;
	::DBus::PropertyAdaptor< std::string > Hostname;

public:

	/* methods exported by this interface,
	 * you will have to implement them in your ObjectAdaptor
	 */
	virtual void SaveHostname(const std::string& hostname) = 0;
	virtual ::DBus::Path AddConnection(const std::map< std::string, std::map< std::string, ::DBus::Variant > >& connection) = 0;
	virtual ::DBus::Path GetConnectionByUuid(const std::string& uuid) = 0;
	virtual std::vector< ::DBus::Path > ListConnections() = 0;

public:

	/* signal emitters for this interface
	 */
	void NewConnection(const ::DBus::Path& arg1)
	{
		::DBus::SignalMessage sig("NewConnection");
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

private:

	/* unmarshalers (to unpack the DBus message before calling the actual interface method)
	 */
	::DBus::Message _SaveHostname_stub(const ::DBus::CallMessage &call)
	{
		::DBus::MessageIter ri = call.reader();

		std::string argin1; ri >> argin1;
		SaveHostname(argin1);
		::DBus::ReturnMessage reply(call);
		return reply;
	}
	::DBus::Message _AddConnection_stub(const ::DBus::CallMessage &call)
	{
		::DBus::MessageIter ri = call.reader();

		std::map< std::string, std::map< std::string, ::DBus::Variant > > argin1; ri >> argin1;
		::DBus::Path argout1 = AddConnection(argin1);
		::DBus::ReturnMessage reply(call);
		::DBus::MessageIter wi = reply.writer();
		wi << argout1;
		return reply;
	}
	::DBus::Message _GetConnectionByUuid_stub(const ::DBus::CallMessage &call)
	{
		::DBus::MessageIter ri = call.reader();

		std::string argin1; ri >> argin1;
		::DBus::Path argout1 = GetConnectionByUuid(argin1);
		::DBus::ReturnMessage reply(call);
		::DBus::MessageIter wi = reply.writer();
		wi << argout1;
		return reply;
	}
	::DBus::Message _ListConnections_stub(const ::DBus::CallMessage &call)
	{
		//::DBus::MessageIter ri = call.reader();

		std::vector< ::DBus::Path > argout1 = ListConnections();
		::DBus::ReturnMessage reply(call);
		::DBus::MessageIter wi = reply.writer();
		wi << argout1;
		return reply;
	}
};

} } } 
#endif //__dbusxx__Settings_adaptor_h__ADAPTOR_MARSHAL_H
