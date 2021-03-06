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

#ifndef __dbusxx_____adaptors_AgentManager_hpp__ADAPTOR_MARSHAL_H
#define __dbusxx_____adaptors_AgentManager_hpp__ADAPTOR_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace org {
namespace freedesktop {
namespace NetworkManager {

class AgentManager_adaptor
: public ::DBus::InterfaceAdaptor
{
public:

	AgentManager_adaptor()
	: ::DBus::InterfaceAdaptor("org.freedesktop.NetworkManager.AgentManager")
	{
		register_method(AgentManager_adaptor, Register, _Register_stub);
		register_method(AgentManager_adaptor, Unregister, _Unregister_stub);
	}

	::DBus::IntrospectedInterface *introspect() const 
	{
		static ::DBus::IntrospectedArgument Register_args[] = 
		{
			{ "identifier", "s", true },
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedArgument Unregister_args[] = 
		{
			{ 0, 0, 0 }
		};
		static ::DBus::IntrospectedMethod AgentManager_adaptor_methods[] = 
		{
			{ "Register", Register_args },
			{ "Unregister", Unregister_args },
			{ 0, 0 }
		};
		static ::DBus::IntrospectedMethod AgentManager_adaptor_signals[] = 
		{
			{ 0, 0 }
		};
		static ::DBus::IntrospectedProperty AgentManager_adaptor_properties[] = 
		{
			{ 0, 0, 0, 0 }
		};
		static ::DBus::IntrospectedInterface AgentManager_adaptor_interface = 
		{
			"org.freedesktop.NetworkManager.AgentManager",
			AgentManager_adaptor_methods,
			AgentManager_adaptor_signals,
			AgentManager_adaptor_properties
		};
		return &AgentManager_adaptor_interface;
	}

public:

	/* properties exposed by this interface, use
	 * property() and property(value) to get and set a particular property
	 */

public:

	/* methods exported by this interface,
	 * you will have to implement them in your ObjectAdaptor
	 */
	virtual void Register(const std::string& identifier) = 0;
	virtual void Unregister() = 0;

public:

	/* signal emitters for this interface
	 */

private:

	/* unmarshalers (to unpack the DBus message before calling the actual interface method)
	 */
	::DBus::Message _Register_stub(const ::DBus::CallMessage &call)
	{
		::DBus::MessageIter ri = call.reader();

		std::string argin1; ri >> argin1;
		Register(argin1);
		::DBus::ReturnMessage reply(call);
		return reply;
	}
	::DBus::Message _Unregister_stub(const ::DBus::CallMessage &call)
	{
		//::DBus::MessageIter ri = call.reader();

		Unregister();
		::DBus::ReturnMessage reply(call);
		return reply;
	}
};

} } } 
#endif //__dbusxx_____adaptors_AgentManager_hpp__ADAPTOR_MARSHAL_H
