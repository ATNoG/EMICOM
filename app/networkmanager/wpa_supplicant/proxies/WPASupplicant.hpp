//==============================================================================
// Brief   : wpa_supplicant main D-Bus interface proxy
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

#ifndef __dbusxx_____proxies_WPASupplicant_hpp__PROXY_MARSHAL_H
#define __dbusxx_____proxies_WPASupplicant_hpp__PROXY_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace fi {
namespace w1 {

class wpa_supplicant1_proxy
: public ::DBus::InterfaceProxy
{
public:

	wpa_supplicant1_proxy()
	: ::DBus::InterfaceProxy("fi.w1.wpa_supplicant1")
	{
		connect_signal(wpa_supplicant1_proxy, InterfaceAdded, _InterfaceAdded_stub);
		connect_signal(wpa_supplicant1_proxy, InterfaceRemoved, _InterfaceRemoved_stub);
		connect_signal(wpa_supplicant1_proxy, PropertiesChanged, _PropertiesChanged_stub);
	}

public:

	/* properties exported by this interface */
		const std::string DebugLevel() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1";
			const std::string property_name  = "DebugLevel";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		void DebugLevel( const std::string & input) {
			::DBus::CallMessage call ;
			 call.member("Set");  call.interface( "org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			::DBus::Variant value;
			::DBus::MessageIter vi = value.writer ();
			vi << input;
			const std::string interface_name = "fi.w1.wpa_supplicant1";
			const std::string property_name  = "DebugLevel";
			wi << interface_name;
			wi << property_name;
			wi << value;
			::DBus::Message ret = this->invoke_method (call);
		};
		const bool DebugTimestamp() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1";
			const std::string property_name  = "DebugTimestamp";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		void DebugTimestamp( const bool & input) {
			::DBus::CallMessage call ;
			 call.member("Set");  call.interface( "org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			::DBus::Variant value;
			::DBus::MessageIter vi = value.writer ();
			vi << input;
			const std::string interface_name = "fi.w1.wpa_supplicant1";
			const std::string property_name  = "DebugTimestamp";
			wi << interface_name;
			wi << property_name;
			wi << value;
			::DBus::Message ret = this->invoke_method (call);
		};
		const bool DebugShowKeys() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1";
			const std::string property_name  = "DebugShowKeys";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		void DebugShowKeys( const bool & input) {
			::DBus::CallMessage call ;
			 call.member("Set");  call.interface( "org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			::DBus::Variant value;
			::DBus::MessageIter vi = value.writer ();
			vi << input;
			const std::string interface_name = "fi.w1.wpa_supplicant1";
			const std::string property_name  = "DebugShowKeys";
			wi << interface_name;
			wi << property_name;
			wi << value;
			::DBus::Message ret = this->invoke_method (call);
		};
		const std::vector< ::DBus::Path > Interfaces() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1";
			const std::string property_name  = "Interfaces";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		const std::vector< std::string > EapMethods() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1";
			const std::string property_name  = "EapMethods";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
public:

	/* methods exported by this interface,
	 * this functions will invoke the corresponding methods on the remote objects
	 */
	::DBus::Path CreateInterface(const std::map< std::string, ::DBus::Variant >& args)
	{
		::DBus::CallMessage call;
		::DBus::MessageIter wi = call.writer();

		wi << args;
		call.member("CreateInterface");
		::DBus::Message ret = invoke_method (call);
		::DBus::MessageIter ri = ret.reader();

		::DBus::Path argout;
		ri >> argout;
		return argout;
	}

	void RemoveInterface(const ::DBus::Path& path)
	{
		::DBus::CallMessage call;
		::DBus::MessageIter wi = call.writer();

		wi << path;
		call.member("RemoveInterface");
		::DBus::Message ret = invoke_method (call);
	}

	::DBus::Path GetInterface(const std::string& ifname)
	{
		::DBus::CallMessage call;
		::DBus::MessageIter wi = call.writer();

		wi << ifname;
		call.member("GetInterface");
		::DBus::Message ret = invoke_method (call);
		::DBus::MessageIter ri = ret.reader();

		::DBus::Path argout;
		ri >> argout;
		return argout;
	}


public:

	/* signal handlers for this interface
	 */
	virtual void InterfaceAdded(const ::DBus::Path& path, const std::map< std::string, ::DBus::Variant >& properties) = 0;
	virtual void InterfaceRemoved(const ::DBus::Path& path) = 0;
	virtual void PropertiesChanged(const std::map< std::string, ::DBus::Variant >& properties) = 0;

private:

	/* unmarshalers (to unpack the DBus message before calling the actual signal handler)
	 */
	void _InterfaceAdded_stub(const ::DBus::SignalMessage &sig)
	{
		::DBus::MessageIter ri = sig.reader();

		::DBus::Path path;
		ri >> path;
		std::map< std::string, ::DBus::Variant > properties;
		ri >> properties;
		InterfaceAdded(path, properties);
	}
	void _InterfaceRemoved_stub(const ::DBus::SignalMessage &sig)
	{
		::DBus::MessageIter ri = sig.reader();

		::DBus::Path path;
		ri >> path;
		InterfaceRemoved(path);
	}
	void _PropertiesChanged_stub(const ::DBus::SignalMessage &sig)
	{
		::DBus::MessageIter ri = sig.reader();

		std::map< std::string, ::DBus::Variant > properties;
		ri >> properties;
		PropertiesChanged(properties);
	}
};

} } 
#endif //__dbusxx_____proxies_WPASupplicant_hpp__PROXY_MARSHAL_H
