//==============================================================================
// Brief   : dhcpcd main D-Bus datatype proxy
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

#ifndef __ODTONE_DHCP_DHCPCD_
#define __ODTONE_DHCP_DHCPCD_

#include "proxies/dhcpcd.hpp"
#include <boost/noncopyable.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/function.hpp>
#include <odtone/mih/types/ipconfig.hpp>

using namespace name::marples::roy;

namespace odtone {
namespace dhcp {

class dhcpcd : boost::noncopyable,
	public name::marples::roy::dhcpcd_proxy,
	public DBus::IntrospectableProxy,
	public DBus::ObjectProxy
{
	typedef boost::function<void(bool success,
	                             const boost::optional<mih::ip_info_list> &address_list,
	                             const boost::optional<mih::ip_info_list> &route_list,
	                             const boost::optional<mih::ip_addr_list> &dns_list,
	                             const boost::optional<mih::fqdn_list> &domain_list)> completion_handler;

public:

	/**
	 * Public interface constructor.
	 */
	dhcpcd(DBus::Connection &connection, const char *path, const char *name);

	/**
	 * Add a new completion handler for an interface.

	 * It is removed once called.
	 *
	 * @param h The callback handler.
	 */
	void add_completion_handler(const std::string &interface, const completion_handler &h);

	/**
	 *
	 */
	void Event(const std::map< std::string, ::DBus::Variant >& configuration);

	/**
	 *
	 */
	void StatusChanged(const std::string& status);

	/**
	 *
	 */
	void ScanResultsSignal(const std::string& interface);

private:
	boost::shared_mutex                                    _completion_handlers_mutex;
	std::map<std::string, std::vector<completion_handler>> _completion_handlers;
};

}; };

#endif /* __ODTONE_DHCP_DHCPCD_ */
