//==============================================================================
// Brief   : wpa_supplicant Interface D-Bus interface proxy
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

#ifndef __dbusxx_____proxies_Interface_hpp__PROXY_MARSHAL_H
#define __dbusxx_____proxies_Interface_hpp__PROXY_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace fi {
namespace w1 {
namespace wpa_supplicant1 {

class Interface_proxy
: public ::DBus::InterfaceProxy
{
public:

	Interface_proxy()
	: ::DBus::InterfaceProxy("fi.w1.wpa_supplicant1.Interface")
	{
		connect_signal(Interface_proxy, ScanDone, _ScanDone_stub);
		connect_signal(Interface_proxy, BSSAdded, _BSSAdded_stub);
		connect_signal(Interface_proxy, BSSRemoved, _BSSRemoved_stub);
		connect_signal(Interface_proxy, BlobAdded, _BlobAdded_stub);
		connect_signal(Interface_proxy, BlobRemoved, _BlobRemoved_stub);
		connect_signal(Interface_proxy, NetworkAdded, _NetworkAdded_stub);
		connect_signal(Interface_proxy, NetworkRemoved, _NetworkRemoved_stub);
		connect_signal(Interface_proxy, NetworkSelected, _NetworkSelected_stub);
		connect_signal(Interface_proxy, PropertiesChanged, _PropertiesChanged_stub);
	}

public:

	/* properties exported by this interface */
		const std::map< std::string, ::DBus::Variant > Capabilities() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface";
			const std::string property_name  = "Capabilities";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		const std::string State() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface";
			const std::string property_name  = "State";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		const bool Scanning() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface";
			const std::string property_name  = "Scanning";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		const uint32_t ApScan() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface";
			const std::string property_name  = "ApScan";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		void ApScan( const uint32_t & input) {
			::DBus::CallMessage call ;
			 call.member("Set");  call.interface( "org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			::DBus::Variant value;
			::DBus::MessageIter vi = value.writer ();
			vi << input;
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface";
			const std::string property_name  = "ApScan";
			wi << interface_name;
			wi << property_name;
			wi << value;
			::DBus::Message ret = this->invoke_method (call);
		};
		const std::string Ifname() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface";
			const std::string property_name  = "Ifname";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		const std::string Driver() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface";
			const std::string property_name  = "Driver";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		const std::string BridgeIfname() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface";
			const std::string property_name  = "BridgeIfname";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		const ::DBus::Path CurrentBSS() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface";
			const std::string property_name  = "CurrentBSS";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		const ::DBus::Path CurrentNetwork() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface";
			const std::string property_name  = "CurrentNetwork";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		const std::map< std::string, std::vector< uint8_t > > Blobs() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface";
			const std::string property_name  = "Blobs";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		const std::vector< ::DBus::Path > BSSs() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface";
			const std::string property_name  = "BSSs";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		const uint32_t BSSExpireAge() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface";
			const std::string property_name  = "BSSExpireAge";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		void BSSExpireAge( const uint32_t & input) {
			::DBus::CallMessage call ;
			 call.member("Set");  call.interface( "org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			::DBus::Variant value;
			::DBus::MessageIter vi = value.writer ();
			vi << input;
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface";
			const std::string property_name  = "BSSExpireAge";
			wi << interface_name;
			wi << property_name;
			wi << value;
			::DBus::Message ret = this->invoke_method (call);
		};
		const uint32_t BSSExpireCount() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface";
			const std::string property_name  = "BSSExpireCount";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		void BSSExpireCount( const uint32_t & input) {
			::DBus::CallMessage call ;
			 call.member("Set");  call.interface( "org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			::DBus::Variant value;
			::DBus::MessageIter vi = value.writer ();
			vi << input;
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface";
			const std::string property_name  = "BSSExpireCount";
			wi << interface_name;
			wi << property_name;
			wi << value;
			::DBus::Message ret = this->invoke_method (call);
		};
		const std::string Country() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface";
			const std::string property_name  = "Country";
			wi << interface_name;
			wi << property_name;
			::DBus::Message ret = this->invoke_method (call);
			::DBus::MessageIter ri = ret.reader ();
			::DBus::Variant argout; 
			ri >> argout;
			return argout;
		};
		void Country( const std::string & input) {
			::DBus::CallMessage call ;
			 call.member("Set");  call.interface( "org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			::DBus::Variant value;
			::DBus::MessageIter vi = value.writer ();
			vi << input;
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface";
			const std::string property_name  = "Country";
			wi << interface_name;
			wi << property_name;
			wi << value;
			::DBus::Message ret = this->invoke_method (call);
		};
		const std::vector< ::DBus::Path > Networks() {
			::DBus::CallMessage call ;
			 call.member("Get"); call.interface("org.freedesktop.DBus.Properties");
			::DBus::MessageIter wi = call.writer(); 
			const std::string interface_name = "fi.w1.wpa_supplicant1.Interface";
			const std::string property_name  = "Networks";
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
	void Scan(const std::map< std::string, ::DBus::Variant >& args)
	{
		::DBus::CallMessage call;
		::DBus::MessageIter wi = call.writer();

		wi << args;
		call.member("Scan");
		::DBus::Message ret = invoke_method (call);
	}

	void Disconnect()
	{
		::DBus::CallMessage call;
		call.member("Disconnect");
		::DBus::Message ret = invoke_method (call);
	}

	::DBus::Path AddNetwork(const std::map< std::string, ::DBus::Variant >& args)
	{
		::DBus::CallMessage call;
		::DBus::MessageIter wi = call.writer();

		wi << args;
		call.member("AddNetwork");
		::DBus::Message ret = invoke_method (call);
		::DBus::MessageIter ri = ret.reader();

		::DBus::Path argout;
		ri >> argout;
		return argout;
	}

	void RemoveNetwork(const ::DBus::Path& path)
	{
		::DBus::CallMessage call;
		::DBus::MessageIter wi = call.writer();

		wi << path;
		call.member("RemoveNetwork");
		::DBus::Message ret = invoke_method (call);
	}

	void SelectNetwork(const ::DBus::Path& path)
	{
		::DBus::CallMessage call;
		::DBus::MessageIter wi = call.writer();

		wi << path;
		call.member("SelectNetwork");
		::DBus::Message ret = invoke_method (call);
	}

	void AddBlob(const std::string& name, const std::vector< uint8_t >& data)
	{
		::DBus::CallMessage call;
		::DBus::MessageIter wi = call.writer();

		wi << name;
		wi << data;
		call.member("AddBlob");
		::DBus::Message ret = invoke_method (call);
	}

	std::vector< uint8_t > GetBlob(const std::string& name)
	{
		::DBus::CallMessage call;
		::DBus::MessageIter wi = call.writer();

		wi << name;
		call.member("GetBlob");
		::DBus::Message ret = invoke_method (call);
		::DBus::MessageIter ri = ret.reader();

		std::vector< uint8_t > argout;
		ri >> argout;
		return argout;
	}

	void RemoveBlob(const std::string& name)
	{
		::DBus::CallMessage call;
		::DBus::MessageIter wi = call.writer();

		wi << name;
		call.member("RemoveBlob");
		::DBus::Message ret = invoke_method (call);
	}


public:

	/* signal handlers for this interface
	 */
	virtual void ScanDone(const bool& success) = 0;
	virtual void BSSAdded(const ::DBus::Path& path, const std::map< std::string, ::DBus::Variant >& properties) = 0;
	virtual void BSSRemoved(const ::DBus::Path& path) = 0;
	virtual void BlobAdded(const std::string& name) = 0;
	virtual void BlobRemoved(const std::string& name) = 0;
	virtual void NetworkAdded(const ::DBus::Path& path, const std::map< std::string, ::DBus::Variant >& properties) = 0;
	virtual void NetworkRemoved(const ::DBus::Path& path) = 0;
	virtual void NetworkSelected(const ::DBus::Path& path) = 0;
	virtual void PropertiesChanged(const std::map< std::string, ::DBus::Variant >& properties) = 0;

private:

	/* unmarshalers (to unpack the DBus message before calling the actual signal handler)
	 */
	void _ScanDone_stub(const ::DBus::SignalMessage &sig)
	{
		::DBus::MessageIter ri = sig.reader();

		bool success;
		ri >> success;
		ScanDone(success);
	}
	void _BSSAdded_stub(const ::DBus::SignalMessage &sig)
	{
		::DBus::MessageIter ri = sig.reader();

		::DBus::Path path;
		ri >> path;
		std::map< std::string, ::DBus::Variant > properties;
		ri >> properties;
		BSSAdded(path, properties);
	}
	void _BSSRemoved_stub(const ::DBus::SignalMessage &sig)
	{
		::DBus::MessageIter ri = sig.reader();

		::DBus::Path path;
		ri >> path;
		BSSRemoved(path);
	}
	void _BlobAdded_stub(const ::DBus::SignalMessage &sig)
	{
		::DBus::MessageIter ri = sig.reader();

		std::string name;
		ri >> name;
		BlobAdded(name);
	}
	void _BlobRemoved_stub(const ::DBus::SignalMessage &sig)
	{
		::DBus::MessageIter ri = sig.reader();

		std::string name;
		ri >> name;
		BlobRemoved(name);
	}
	void _NetworkAdded_stub(const ::DBus::SignalMessage &sig)
	{
		::DBus::MessageIter ri = sig.reader();

		::DBus::Path path;
		ri >> path;
		std::map< std::string, ::DBus::Variant > properties;
		ri >> properties;
		NetworkAdded(path, properties);
	}
	void _NetworkRemoved_stub(const ::DBus::SignalMessage &sig)
	{
		::DBus::MessageIter ri = sig.reader();

		::DBus::Path path;
		ri >> path;
		NetworkRemoved(path);
	}
	void _NetworkSelected_stub(const ::DBus::SignalMessage &sig)
	{
		::DBus::MessageIter ri = sig.reader();

		::DBus::Path path;
		ri >> path;
		NetworkSelected(path);
	}
	void _PropertiesChanged_stub(const ::DBus::SignalMessage &sig)
	{
		::DBus::MessageIter ri = sig.reader();

		std::map< std::string, ::DBus::Variant > properties;
		ri >> properties;
		PropertiesChanged(properties);
	}
};

} } } 
#endif //__dbusxx_____proxies_Interface_hpp__PROXY_MARSHAL_H
