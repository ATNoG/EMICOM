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

#ifndef WPA_SUPPLICANT_INTERFACE

#include "proxies/Interface.hpp"
#include <boost/noncopyable.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/function.hpp>

using namespace fi::w1::wpa_supplicant1;

namespace odtone {
namespace wpa_supplicant {

class Interface : boost::noncopyable,
	public Interface_proxy,
	public DBus::IntrospectableProxy,
	public DBus::ObjectProxy
{
	typedef boost::function<void(bool success)> completion_handler;

public:

	/**
	 * Public interface constructor.
	 */
	Interface(DBus::Connection &connection, const char *path, const char *name);

	/**
	 * Add a new completion handler for this connection.
	 * It is only called with either a "completed" or "disconnected" state.
	 * The call is only processed if the Interface as been through the "associating" state.
	 * It is removed once called.
	 *
	 * @param h The callback handler.
	 */
	void add_completion_handler(const completion_handler &h);

	/**
	 * Scan finish event handler.
	 */
	void ScanDone(const bool& success);

	/**
	 * BSS Added event handler.
	 */
	void BSSAdded(const ::DBus::Path& path, const std::map< std::string, ::DBus::Variant >& properties);

	/**
	 * BSS Removed event handler.
	 */
	void BSSRemoved(const ::DBus::Path& path);

	/**
	 * BlobAdded event handler.
	 */
	void BlobAdded(const std::string& name);

	/**
	 * BlobRemoved event handler.
	 */
	void BlobRemoved(const std::string& name);

	/**
	 * NetworkAdded event handler.
	 */
	void NetworkAdded(const ::DBus::Path& path, const std::map< std::string, ::DBus::Variant >& properties);

	/**
	 * NetworkRemoved event handler.
	 */
	void NetworkRemoved(const ::DBus::Path& path);

	/**
	 * NetworkSelected event handler.
	 */
	void NetworkSelected(const ::DBus::Path& path);

	/**
	 * PropertiesChanged event handler.
	 */
	void PropertiesChanged(const std::map< std::string, ::DBus::Variant >& properties);

	/**
	 * NetworkRequest event handler.
	 */
	void NetworkRequest(const ::DBus::Path& path, const std::string &field, const std::string &txt);

private:
	boost::shared_mutex             _completion_handlers_mutex;
	std::vector<completion_handler> _buffered_completion_handlers;
	std::vector<completion_handler> _completion_handlers;
	bool                            _wired;
};

}; };

#endif /* WPA_SUPPLICANT_INTERFACE */
