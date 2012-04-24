//==============================================================================
// Brief   : wpa_supplicant Interface.WPS D-Bus interface proxy
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

#ifndef __dbusxx_____proxies_WPS_hpp__PROXY_MARSHAL_H
#define __dbusxx_____proxies_WPS_hpp__PROXY_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace fi {
namespace w1 {
namespace wpa_supplicant1 {
namespace Interface {

class WPS_proxy
: public ::DBus::InterfaceProxy
{
public:

	WPS_proxy()
	: ::DBus::InterfaceProxy("fi.w1.wpa_supplicant1.Interface.WPS")
	{
		connect_signal(WPS_proxy, Event, _Event_stub);
		connect_signal(WPS_proxy, Credentials, _Credentials_stub);
		connect_signal(WPS_proxy, PropertiesChanged, _PropertiesChanged_stub);
	}

public:

	/* properties exported by this interface */
		const bool ProcessCredentials() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface.WPS";
			const std::string property_name  = "ProcessCredentials";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		void ProcessCredentials( const bool & input) {
			::DBus::CallMessage call ;
			 call.member("Set");  call.interface( "org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			::DBus::Variant value;
			::DBus::MessageIter vi = value.writer ();
			vi << input;
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface.WPS";
			const std::string property_name  = "ProcessCredentials";
			wi << interface_name;
			wi << property_name;
			wi << value;
			::DBus::Message ret = this->invoke_method (call);
		};
public:

	/* methods exported by this interface,
	 * this functions will invoke the corresponding methods on the remote objects
	 */
	std::map< std::string, ::DBus::Variant > Start(const std::map< std::string, ::DBus::Variant >& args)
	{
		::DBus::CallMessage call;
		::DBus::MessageIter wi = call.writer();

		wi << args;
		call.member("Start");
		::DBus::Message ret = invoke_method (call);
		::DBus::MessageIter ri = ret.reader();

		std::map< std::string, ::DBus::Variant > argout;
		ri >> argout;
		return argout;
	}


public:

	/* signal handlers for this interface
	 */
	virtual void Event(const std::string& name, const std::map< std::string, ::DBus::Variant >& args) = 0;
	virtual void Credentials(const std::map< std::string, ::DBus::Variant >& credentials) = 0;
	virtual void PropertiesChanged() = 0;

private:

	/* unmarshalers (to unpack the DBus message before calling the actual signal handler)
	 */
	void _Event_stub(const ::DBus::SignalMessage &sig)
	{
		::DBus::MessageIter ri = sig.reader();

		std::string name;
		ri >> name;
		std::map< std::string, ::DBus::Variant > args;
		ri >> args;
		Event(name, args);
	}
	void _Credentials_stub(const ::DBus::SignalMessage &sig)
	{
		::DBus::MessageIter ri = sig.reader();

		std::map< std::string, ::DBus::Variant > credentials;
		ri >> credentials;
		Credentials(credentials);
	}
	void _PropertiesChanged_stub(const ::DBus::SignalMessage &sig)
	{
		PropertiesChanged();
	}
};

} } } } 
#endif //__dbusxx_____proxies_WPS_hpp__PROXY_MARSHAL_H
