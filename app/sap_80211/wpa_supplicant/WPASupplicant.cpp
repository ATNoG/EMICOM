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


#include "WPASupplicant.hpp"

namespace odtone {
namespace wpa_supplicant {

WPASupplicant::WPASupplicant(DBus::Connection &connection, const char *path, const char *name)
	: DBus::ObjectProxy(connection, path, name)
{
}

void WPASupplicant::InterfaceAdded(const ::DBus::Path& path, const std::map< std::string, ::DBus::Variant >& properties)
{
}

void WPASupplicant::InterfaceRemoved(const ::DBus::Path& path)
{
}

void WPASupplicant::PropertiesChanged(const std::map< std::string, ::DBus::Variant >& properties)
{
}

}; };
