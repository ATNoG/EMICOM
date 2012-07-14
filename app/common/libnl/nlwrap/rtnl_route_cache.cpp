//=============================================================================
// Brief   : RTNetlink route cache RAI wrapper
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

#include "rtnl_route_cache.hpp"

#include <boost/lexical_cast.hpp>
#include <stdexcept>

namespace nlwrap {

rtnl_route_cache::rtnl_route_cache()
{
	int result = ::rtnl_route_alloc_cache(_socket, AF_UNSPEC, 0, &_cache);
	if (result) {
		throw std::runtime_error("Error allocating route cache: " + boost::lexical_cast<std::string>(result));
	}
}

rtnl_route_cache::~rtnl_route_cache()
{
	if (_cache) {
		::nl_cache_free(_cache);
	}
}

void rtnl_route_cache::clear(rtnl_route &route)
{
	// this ought to be enough, but it fails too often!!
	//int result = ::rtnl_route_delete(_socket, route, 0);

	::nl_object *match;
	::rtnl_route *filter = route;
	int result = 0;

	for (match = ::nl_cache_get_first(_cache); match && !result; match = ::nl_cache_get_next(match)) {
		if (::nl_object_match_filter(match, reinterpret_cast< ::nl_object * >(filter))) {
			result = ::rtnl_route_delete(_socket, reinterpret_cast< ::rtnl_route * >(match), 0);
		}
	}

	if (result) {
		throw std::runtime_error("Unable to delete route: " + boost::lexical_cast<std::string>(result));
	}
}

void rtnl_route_cache::add(rtnl_route &route)
{
	int result = ::rtnl_route_add(_socket, route, NLM_F_REPLACE);
	if (result) {
		throw std::runtime_error("Unable to add route: " + boost::lexical_cast<std::string>(result));
	}
}

}

// EOF ////////////////////////////////////////////////////////////////////////
