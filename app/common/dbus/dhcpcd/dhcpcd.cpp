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

#include "dhcpcd.hpp"

#include <boost/algorithm/string.hpp>

namespace odtone {
namespace dhcp {

dhcpcd::dhcpcd(DBus::Connection &connection, const char *path, const char *name)
	: DBus::ObjectProxy(connection, path, name)
{
}

void dhcpcd::add_completion_handler(const std::string &interface, const completion_handler &h)
{
	boost::unique_lock<boost::shared_mutex> lock(_completion_handlers_mutex);
	_completion_handlers[interface].push_back(h);
}

void dhcpcd::Event(const std::map< std::string, ::DBus::Variant >& configuration)
{
	std::cerr << "Event" << std::endl;

	boost::unique_lock<boost::shared_mutex> lock(_completion_handlers_mutex);

	if (_completion_handlers.empty()) {
		return;
	}

	auto interface_it = configuration.find("Interface");
	if (interface_it == configuration.end()) {
		return;
	}
	std::string interface = interface_it->second;

	auto handlers_it = _completion_handlers.find(interface);
	if (handlers_it == _completion_handlers.end()) {
		return;
	}

	auto reason_it = configuration.find("Reason");
	if (reason_it == configuration.end()) {
		return;
	}
	std::string reason = reason_it->second;
	if (   !boost::iequals(reason, "BOUND")
	    && !boost::iequals(reason, "RENEW")
	    && !boost::iequals(reason, "REBOOT")) {
		return;
	}

	try {
		bool success = false;
		mih::ip_info_list address_list;
		mih::ip_info_list route_list;
		mih::ip_addr_list dns_list;
		mih::fqdn_list    domain_list;

		auto version_it = configuration.find("Type");
		if (version_it == configuration.end()) {
			return;
		}
		std::string version = version_it->second;
		if (boost::iequals(version, "ipv4")) {
			// copy the addresses
			auto address_it = configuration.find("IPAddress");
			auto prefix_it  = configuration.find("SubnetCIDR");
			auto gateway_it = configuration.find("Routers");
			if (   address_it != configuration.end()
				&& prefix_it  != configuration.end()
				&& gateway_it != configuration.end()) {
				uint32_t address = address_it->second;
				unsigned char prefix = prefix_it->second;
				std::vector<uint32_t> routers = gateway_it->second;

				if (routers.size() != 0) {
					mih::ip_info addr;
					addr.subnet.ipaddr = mih::ip_addr(mih::ip_addr::ipv4, &address, sizeof(uint32_t));
					addr.subnet.ipprefixlen = prefix;
					addr.gateway = mih::ip_addr(mih::ip_addr::ipv4, &routers[0], sizeof(uint32_t));

					address_list.push_back(addr);
				}

				success = true;
			}

			// copy the route list
			// TODO

			// copy the dns server list
			auto dns_it = configuration.find("DomainNameServers");
			if (dns_it != configuration.end()) {
				std::vector<uint32_t> servers = dns_it->second;
				for(auto server = servers.begin(); server != servers.end(); ++server) {
					uint32_t dns = *server;
					mih::ip_addr addr(mih::ip_addr::ipv4, &dns, sizeof(uint32_t));
					dns_list.push_back(addr);
				}
			}

			// copy the search domain list
			auto search_it = configuration.find("DomainSearch");
			if (search_it != configuration.end()) {
				std::vector<std::string> domains = search_it->second;
				domain_list.insert(domain_list.end(), domains.begin(), domains.end());
			}
		} else { // ipv6
			// TODO
		}

		for (auto h = handlers_it->second.begin(); h != handlers_it->second.end(); ++h) {
			(*h)(success,
				 boost::make_optional(success && address_list.size() > 0, address_list),
				 boost::make_optional(success && route_list.size()   > 0, route_list),
				 boost::make_optional(success && dns_list.size()     > 0, dns_list),
				 boost::make_optional(success && domain_list.size()  > 0, domain_list));
		}
	} catch (std::exception &e) {
		for (auto h = handlers_it->second.begin(); h != handlers_it->second.end(); ++h) {
			(*h)(false, boost::none, boost::none, boost::none, boost::none);
		}
	}

	_completion_handlers.clear();
}

void dhcpcd::StatusChanged(const std::string& status)
{
}

void dhcpcd::ScanResultsSignal(const std::string& interface)
{
}

}; };
