//==============================================================================
// Brief   : DHClient data type
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

#ifndef DHCLIENT_EXECUTABLE
#define DHCLIENT_EXECUTABLE "/usr/sbin/dhclient"
#endif /* DHCLIENT_EXECUTABLE */

#include "dhclient.hpp"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

namespace odtone {
namespace dhcp {

dhclient::dhclient(const std::string &dev) : _dev(dev)
{
}

dhclient::~dhclient()
{
}

void dhclient::bind(DHCP_VERSION v)
{
	std::stringstream pidfile;
	pidfile << "dhclient_" << _dev << "_" << v << ".pid";

	std::stringstream command;
	command << DHCLIENT_EXECUTABLE
	        << " -" << v
	        << " -pf " << pidfile.str()
	        << " -1 " << _dev;

	int result = system(command.str().c_str());
	if (result) {
		throw std::runtime_error("Error configuring dynamic address: " + boost::lexical_cast<std::string>(result));
	}
}

void dhclient::release(DHCP_VERSION v)
{
	std::stringstream pidfile;
	pidfile << "dhclient_" << _dev << "_" << v << ".pid";

	std::stringstream command;
	command << DHCLIENT_EXECUTABLE
	        << " -x"
	        << " -pf " << pidfile.str()
	        << " -n"; // do not "configure" any interface

	int result = system(command.str().c_str());
	if (result) {
		throw std::runtime_error("Error releasing dynamic address: " + boost::lexical_cast<std::string>(result));
	}
}

} }

