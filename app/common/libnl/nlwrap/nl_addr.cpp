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

#include "nl_addr.hpp"

#include <stdexcept>

namespace nlwrap {

nl_addr::nl_addr(ADDRESS_FAMILY fam, void *raw, unsigned int size)
{
	_addr = ::nl_addr_build(fam, raw, size);
	if (!_addr) {
		throw std::runtime_error("Error allocating rtnl_addr");
	}

	_own = true;
}

nl_addr::nl_addr(ADDRESS_FAMILY fam, void *raw, unsigned int size, int prefixlen)
{
	_addr = ::nl_addr_build(fam, raw, size);
	if (!_addr) {
		throw std::runtime_error("Error allocating rtnl_addr");
	}

	_own = true;

	::nl_addr_set_prefixlen(_addr, prefixlen);
}

nl_addr::nl_addr(::nl_addr *addr)
{
	_addr = addr;
	_own = false;
}

nl_addr::nl_addr(const nl_addr &copy)
{
	_addr = ::nl_addr_clone(copy._addr);
	if (!_addr) {
		throw std::runtime_error("Error cloning nl_addr");
	}

	_own = true;
}

nl_addr::~nl_addr()
{
	if (_own && _addr) {
		::nl_addr_put(_addr);
	}
}

nl_addr::operator ::nl_addr *()
{
	return _addr;
}

nl_addr &nl_addr::operator =(::nl_addr *addr)
{
	if (_own && _addr) {
		::nl_addr_put(_addr);
	}

	_addr = addr;
	_own = false;

	return *this;
}

void nl_addr::set_prefixlen(int len)
{
	::nl_addr_set_prefixlen(_addr, len);
}

}

// EOF ////////////////////////////////////////////////////////////////////////
