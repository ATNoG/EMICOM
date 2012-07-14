//=============================================================================
// Brief   : RTNetlink address RAI wrapper
// Authors : André Prata <andreprata@av.it.pt>
//-----------------------------------------------------------------------------
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

#include "rtnl_addr.hpp"

#include <stdexcept>

namespace nlwrap {

rtnl_addr::rtnl_addr()
{
	_addr = ::rtnl_addr_alloc();
	if (!_addr) {
		throw std::runtime_error("Error allocating rtnl_addr");
	}

	_own = true;
}

rtnl_addr::rtnl_addr(::rtnl_addr *addr)
{
	_addr = addr;
	_own = false;
}

rtnl_addr::~rtnl_addr()
{
	if (_own && _addr) {
		::rtnl_addr_put(_addr);
	}
}

rtnl_addr::operator ::rtnl_addr *()
{
	return _addr;
}

rtnl_addr &rtnl_addr::operator =(::rtnl_addr *addr)
{
	if (_own && _addr) {
		::rtnl_addr_put(_addr);
	}

	_addr = addr;
	_own = false;

	return *this;
}

void rtnl_addr::set_ifindex(int ifindex)
{
	::rtnl_addr_set_ifindex(_addr, ifindex);
}

void rtnl_addr::set_family(nl_addr::ADDRESS_FAMILY fam)
{
	::rtnl_addr_set_family(_addr, fam);
}

void rtnl_addr::set_local(nl_addr &addr)
{
	::rtnl_addr_set_local(_addr, addr);
}

void rtnl_addr::set_peer(nl_addr &addr)
{
	::rtnl_addr_set_peer(_addr, addr);
}

void rtnl_addr::set_prefixlen(int len)
{
	::rtnl_addr_set_prefixlen(_addr, len);
}

void rtnl_addr::set_broadcast(nl_addr &addr)
{
	::rtnl_addr_set_broadcast(_addr, addr);
}

void rtnl_addr::set_multicast(nl_addr &addr)
{
	::rtnl_addr_set_multicast(_addr, addr);
}

void rtnl_addr::set_anycast(nl_addr &addr)
{
	::rtnl_addr_set_anycast(_addr, addr);
}

void rtnl_addr::set_scope(ADDRESS_SCOPE sc)
{
	::rtnl_addr_set_scope(_addr, sc);
}

}

// EOF ////////////////////////////////////////////////////////////////////////
