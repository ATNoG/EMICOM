//==============================================================================
// Brief   : DHCP Client data type
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

#ifndef __ODTONE_DHCP_DHCPCLIENT
#define __ODTONE_DHCP_DHCPCLIENT

namespace odtone {
namespace dhcp {

class dhcpclient
{

public:
	enum DHCP_VERSION {
		DHCPv4 = 4,
		DHCPv6 = 6
	};

	virtual void bind(DHCP_VERSION v) = 0;

	virtual void release(DHCP_VERSION v) = 0;

};

} }

#endif /* __ODTONE_DHCP_DHCPCLIENT */
