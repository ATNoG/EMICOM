//==============================================================================
// Brief   : wpa_supplicant Network D-Bus interface proxy
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

#ifndef __dbusxx_____proxies_Network_hpp__PROXY_MARSHAL_H
#define __dbusxx_____proxies_Network_hpp__PROXY_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace fi {
namespace w1 {
namespace wpa_supplicant1 {

class Network_proxy
: public ::DBus::InterfaceProxy
{
public:

	Network_proxy()
	: ::DBus::InterfaceProxy("fi.w1.wpa_supplicant1.Network")
	{
		connect_signal(Network_proxy, PropertiesChanged, _PropertiesChanged_stub);
	}

public:

	/* properties exported by this interface */
		const std::map< std::string, ::DBus::Variant > Properties() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.Network";
			const std::string property_name  = "Properties";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		void Properties( const std::map< std::string, ::DBus::Variant > & input) {
			::DBus::CallMessage call ;
			 call.member("Set");  call.interface( "org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			::DBus::Variant value;
			::DBus::MessageIter vi = value.writer ();
			vi << input;
			const std::string interface_name = "fi.w1.wpa_supplicant1.Network";
			const std::string property_name  = "Properties";
			wi << interface_name;
			wi << property_name;
			wi << value;
			::DBus::Message ret = this->invoke_method (call);
		};
		const bool Enabled() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.Network";
			const std::string property_name  = "Enabled";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		void Enabled( const bool & input) {
			::DBus::CallMessage call ;
			 call.member("Set");  call.interface( "org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			::DBus::Variant value;
			::DBus::MessageIter vi = value.writer ();
			vi << input;
			const std::string interface_name = "fi.w1.wpa_supplicant1.Network";
			const std::string property_name  = "Enabled";
			wi << interface_name;
			wi << property_name;
			wi << value;
			::DBus::Message ret = this->invoke_method (call);
		};
public:

	/* methods exported by this interface,
	 * this functions will invoke the corresponding methods on the remote objects
	 */

public:

	/* signal handlers for this interface
	 */
	virtual void PropertiesChanged(const std::map< std::string, ::DBus::Variant >& properties) = 0;

private:

	/* unmarshalers (to unpack the DBus message before calling the actual signal handler)
	 */
	void _PropertiesChanged_stub(const ::DBus::SignalMessage &sig)
	{
		::DBus::MessageIter ri = sig.reader();

		std::map< std::string, ::DBus::Variant > properties;
		ri >> properties;
		PropertiesChanged(properties);
	}
};

} } } 
#endif //__dbusxx_____proxies_Network_hpp__PROXY_MARSHAL_H
