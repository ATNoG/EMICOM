//=============================================================================
// Brief   : RTNetlink route RAI wrapper
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

#include "rtnl_route.hpp"

#include <stdexcept>

namespace nlwrap {

rtnl_route::rtnl_route()
{
	_route = ::rtnl_route_alloc();
	if (!_route) {
		throw std::runtime_error("Error allocating rtnl_route");
	}

	_own = true;
}

rtnl_route::rtnl_route(int ifindex, nl_addr &dst, nl_addr &gateway, ROUTE_SCOPE scope, ROUTE_FAMILY fam)
{
	_route = ::rtnl_route_alloc();
	if (!_route) {
		throw std::runtime_error("Error allocating rtnl_route");
	}

	_own = true;

	try {
		set_oifindex(ifindex);
		set_dst(dst);
		set_gateway(gateway);
		set_scope(scope);
		set_family(fam);
	} catch (...) {
		::rtnl_route_put(_route);
		throw;
	}
}

rtnl_route::rtnl_route(::rtnl_route *route)
{
	_route = route;
	_own = false;
}

rtnl_route::~rtnl_route()
{
	if (_own && _route) {
		::rtnl_route_put(_route);
	}
}

rtnl_route::operator ::rtnl_route *()
{
	return _route;
}

rtnl_route &rtnl_route::operator =(::rtnl_route *route)
{
	if (_own && _route) {
		::rtnl_route_put(_route);
	}

	_route = route;
	_own = false;

	return *this;
}

void rtnl_route::set_oifindex(int ifindex)
{
	::rtnl_nexthop *nh = NULL;

	nh = ::rtnl_route_nh_alloc();
	if (!nh) {
		throw std::runtime_error("Unable to allocate route nexthop");
	}

	::rtnl_route_nh_set_ifindex(nh, ifindex);
	::rtnl_route_add_nexthop(_route, nh);
}

void rtnl_route::set_dst(nl_addr &addr)
{
	if (::rtnl_route_set_dst(_route, addr)) {
		throw std::runtime_error("Error setting route dst");
	}
}

void rtnl_route::set_gateway(nl_addr &addr)
{
	::rtnl_nexthop *nh = NULL;
	int hops = ::rtnl_route_get_nnexthops(_route);

	if (hops > 0) {
		nh = ::rtnl_route_nexthop_n(_route, 0);
	}

	if (!nh) {
		throw std::runtime_error("Unable to get route next hop");
	}

	::rtnl_route_nh_set_gateway(nh, addr);
}

void rtnl_route::set_scope(ROUTE_SCOPE scope)
{
	::rtnl_route_set_scope(_route, scope);
}

void rtnl_route::set_family(ROUTE_FAMILY fam)
{
	if (::rtnl_route_set_family(_route, fam)) {
		throw std::runtime_error("Error setting route family");
	}
}

void rtnl_route::set_table(ROUTE_TABLE table)
{
	::rtnl_route_set_table(_route, table);
}

}

// EOF ////////////////////////////////////////////////////////////////////////
