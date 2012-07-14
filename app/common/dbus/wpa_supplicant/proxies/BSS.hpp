//==============================================================================
// Brief   : wpa_supplicant BSS D-Bus interface proxy
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

#ifndef __dbusxx_____proxies_BSS_hpp__PROXY_MARSHAL_H
#define __dbusxx_____proxies_BSS_hpp__PROXY_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace fi {
namespace w1 {
namespace wpa_supplicant1 {

class BSS_proxy
: public ::DBus::InterfaceProxy
{
public:

	BSS_proxy()
	: ::DBus::InterfaceProxy("fi.w1.wpa_supplicant1.BSS")
	{
		connect_signal(BSS_proxy, PropertiesChanged, _PropertiesChanged_stub);
	}

public:

	/* properties exported by this interface */
		const std::vector< uint8_t > SSID() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.BSS";
			const std::string property_name  = "SSID";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		const std::vector< uint8_t > BSSID() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.BSS";
			const std::string property_name  = "BSSID";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		const bool Privacy() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.BSS";
			const std::string property_name  = "Privacy";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		const std::string Mode() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.BSS";
			const std::string property_name  = "Mode";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		const int16_t Signal() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.BSS";
			const std::string property_name  = "Signal";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		const uint16_t Frequency() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.BSS";
			const std::string property_name  = "Frequency";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		const std::vector< uint32_t > Rates() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.BSS";
			const std::string property_name  = "Rates";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		const std::map< std::string, ::DBus::Variant > WPA() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.BSS";
			const std::string property_name  = "WPA";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		const std::map< std::string, ::DBus::Variant > RSN() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.BSS";
			const std::string property_name  = "RSN";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		const std::vector< uint8_t > IEs() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.BSS";
			const std::string property_name  = "IEs";
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
#endif //__dbusxx_____proxies_BSS_hpp__PROXY_MARSHAL_H
