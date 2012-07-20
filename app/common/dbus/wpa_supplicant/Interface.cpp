//==============================================================================
// Brief   : wpa_supplicant.interface main D-Bus datatype proxy
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

#include "Interface.hpp"

#include <boost/algorithm/string.hpp>

namespace odtone {
namespace wpa_supplicant {

Interface::Interface(DBus::Connection &connection, const char *path, const char *name)
	: DBus::ObjectProxy(connection, path, name)
{
}

void Interface::add_completion_handler(const completion_handler &h)
{
	boost::unique_lock<boost::shared_mutex> lock(_completion_handlers_mutex);
	_buffered_completion_handlers.push_back(h);
}

void Interface::ScanDone(const bool& success)
{
}

void Interface::BSSAdded(const ::DBus::Path& path, const std::map< std::string, ::DBus::Variant >& properties)
{
}

void Interface::BSSRemoved(const ::DBus::Path& path)
{
}

void Interface::BlobAdded(const std::string& name)
{
}

void Interface::BlobRemoved(const std::string& name)
{
}

void Interface::NetworkAdded(const ::DBus::Path& path, const std::map< std::string, ::DBus::Variant >& properties)
{
}

void Interface::NetworkRemoved(const ::DBus::Path& path)
{
}

void Interface::NetworkSelected(const ::DBus::Path& path)
{
}

void Interface::PropertiesChanged(const std::map< std::string, ::DBus::Variant >& properties)
{

	boost::unique_lock<boost::shared_mutex> lock(_completion_handlers_mutex);

	auto pt = properties.find("State");
	if (pt != properties.end()) {
		DBus::MessageIter v = pt->second.reader();
		std::string value;
		v >> value;

		// The "disconnected" or "completed" states are not considered for completion
		// unless the interface has been in the "associating" state first.
		if (boost::iequals(value, "associating")) {
			_completion_handlers = _buffered_completion_handlers;
			_buffered_completion_handlers.clear();
		} else {

			bool success;
			if ((success = boost::iequals(value, "completed")) || boost::iequals(value, "disconnected")) {
				auto it = _completion_handlers.begin();
				while (it != _completion_handlers.end()) {
					(*it)(success);
					it = _completion_handlers.erase(it);
				}
			}
		}
	}
}

}; };
