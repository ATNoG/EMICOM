//==============================================================================
// Brief   : wpa_supplicant main D-Bus datatype proxy
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

#ifndef WPA_SUPPLICANT_WPASUPPLICANT

#include "proxies/WPASupplicant.hpp"
#include <boost/noncopyable.hpp>

using namespace fi::w1;

namespace odtone {
namespace wpa_supplicant {

class WPASupplicant : boost::noncopyable,
	public wpa_supplicant1_proxy,
	public DBus::IntrospectableProxy,
	public DBus::ObjectProxy
{
public:

	/**
	 * Public interface constructor.
	 */
	WPASupplicant(DBus::Connection &connection, const char *path, const char *name);

	/**
	 * Signal handler for InterfaceAdded event.
	 */
	void InterfaceAdded(const ::DBus::Path& path, const std::map< std::string, ::DBus::Variant >& properties);

	/**
	 * Signal handler for interface removed event.
	 */
	void InterfaceRemoved(const ::DBus::Path& path);

	/**
	 * Signal handler for properties changed event.
	 */
	void PropertiesChanged(const std::map< std::string, ::DBus::Variant >& properties);

};

}; };

#endif /* WPA_SUPPLICANT_WPASUPPLICANT */
