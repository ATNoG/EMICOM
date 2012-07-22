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

#ifndef __ODTONE_DHCP_DHCLIENT
#define __ODTONE_DHCP_DHCLIENT

#include "dhcpclient.hpp"

#include <string>

namespace odtone {
namespace dhcp {

class dhclient : public dhcpclient
{

public:

	dhclient(const std::string &dev);

	~dhclient();

	void bind(DHCP_VERSION v);

	void release(DHCP_VERSION v);

private:
	std::string _dev;
};

} }

#endif /* __ODTONE_DHCP_DHCLIENT */
