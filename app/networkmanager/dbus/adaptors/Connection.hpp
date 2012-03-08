//==============================================================================
// Brief   : NetworkManager Connection D-Bus interface adaptor
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

#ifndef __dbusxx_____adaptors_Connection_hpp__ADAPTOR_MARSHAL_H
#define __dbusxx_____adaptors_Connection_hpp__ADAPTOR_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace org {
namespace freedesktop {
namespace NetworkManager {
namespace Settings {

class Connection_adaptor
: public ::DBus::InterfaceAdaptor
{
public:

	Connection_adaptor()
	: ::DBus::InterfaceAdaptor("org.freedesktop.NetworkManager.Settings.Connection")
	{
		register_method(Connection_adaptor, Update, _Update_stub);
		register_method(Connection_adaptor, Delete, _Delete_stub);
		register_method(Connection_adaptor, GetSettings, _GetSettings_stub);
		register_method(Connection_adaptor, GetSecrets, _GetSecrets_stub);
	}

	::DBus::IntrospectedInterface *introspect() const 
	{
		static ::DBus::IntrospectedArgument Update_args[] = 
		{
			{ "properties", "a{sa{sv}}", true },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument Delete_args[] = 
		{
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument GetSettings_args[] = 
		{
			{ "settings", "a{sa{sv}}", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument GetSecrets_args[] = 
		{
			{ "setting_name", "s", true },
			{ "secrets", "a{sa{sv}}", false },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument Updated_args[] = 
		{
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument Removed_args[] = 
		{
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedMethod Connection_adaptor_methods[] = 
		{
			{ "Update", Update_args },
			{ "Delete", Delete_args },
			{ "GetSettings", GetSettings_args },
			{ "GetSecrets", GetSecrets_args },
			{ 0, 0 }
		};
		static ::DBus::IntrospectedMethod Connection_adaptor_signals[] = 
		{
			{ "Updated", Updated_args },
			{ "Removed", Removed_args },
			{ 0, 0 }
		};
		static ::DBus::IntrospectedProperty Connection_adaptor_properties[] = 
		{
			{ 0, 0, 0, 0 }
		};
		static ::DBus::IntrospectedInterface Connection_adaptor_interface = 
		{
			"org.freedesktop.NetworkManager.Settings.Connection",
			Connection_adaptor_methods,
			Connection_adaptor_signals,
			Connection_adaptor_properties
		};
		return &Connection_adaptor_interface;
	}

public:

	/* properties exposed by this interface, use
	 * property() and property(value) to get and set a particular property
	 */

public:

	/* methods exported by this interface,
	 * you will have to implement them in your ObjectAdaptor
	 */
	virtual void Update(const std::map< std::string, std::map< std::string, ::DBus::Variant > >& properties) = 0;
	virtual void Delete() = 0;
	virtual std::map< std::string, std::map< std::string, ::DBus::Variant > > GetSettings() = 0;
	virtual std::map< std::string, std::map< std::string, ::DBus::Variant > > GetSecrets(const std::string& setting_name) = 0;

public:

	/* signal emitters for this interface
	 */
	void Updated()
	{
		::DBus::SignalMessage sig("Updated");
		emit_signal(sig);
	}
	void Removed()
	{
		::DBus::SignalMessage sig("Removed");
		emit_signal(sig);
	}

private:

	/* unmarshalers (to unpack the DBus message before calling the actual interface method)
	 */
	::DBus::Message _Update_stub(const ::DBus::CallMessage &call)
	{
		::DBus::MessageIter ri = call.reader();

		std::map< std::string, std::map< std::string, ::DBus::Variant > > argin1; ri >> argin1;
		Update(argin1);
		::DBus::ReturnMessage reply(call);
		return reply;
	}
	::DBus::Message _Delete_stub(const ::DBus::CallMessage &call)
	{
		//::DBus::MessageIter ri = call.reader();

		Delete();
		::DBus::ReturnMessage reply(call);
		return reply;
	}
	::DBus::Message _GetSettings_stub(const ::DBus::CallMessage &call)
	{
		//::DBus::MessageIter ri = call.reader();

		std::map< std::string, std::map< std::string, ::DBus::Variant > > argout1 = GetSettings();
		::DBus::ReturnMessage reply(call);
		::DBus::MessageIter wi = reply.writer();
		wi << argout1;
		return reply;
	}
	::DBus::Message _GetSecrets_stub(const ::DBus::CallMessage &call)
	{
		::DBus::MessageIter ri = call.reader();

		std::string argin1; ri >> argin1;
		std::map< std::string, std::map< std::string, ::DBus::Variant > > argout1 = GetSecrets(argin1);
		::DBus::ReturnMessage reply(call);
		::DBus::MessageIter wi = reply.writer();
		wi << argout1;
		return reply;
	}
};

} } } } 
#endif //__dbusxx_____adaptors_Connection_hpp__ADAPTOR_MARSHAL_H
