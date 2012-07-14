//=============================================================================
// Brief   : RTNetlink addr cache RAI wrapper
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

#include "rtnl_addr_cache.hpp"

#include <stdexcept>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <string>
namespace nlwrap {

rtnl_addr_cache::rtnl_addr_cache()
{
	int result = ::rtnl_addr_alloc_cache(_socket, &_cache);
	if (result) {
		throw std::runtime_error("Error allocating addr cache: " + boost::lexical_cast<std::string>(result));
	}
}

rtnl_addr_cache::~rtnl_addr_cache()
{
	if (_cache) {
		::nl_cache_free(_cache);
	}
}

void rtnl_addr_cache::clear(rtnl_addr &addr)
{
	// this ought to be enough, but it fails too often!!
	//int result = ::rtnl_addr_delete(_socket, addr, 0);

	::nl_object *match;
	::rtnl_addr *filter = addr;
	int result = 0;

	for (match = ::nl_cache_get_first(_cache); match && !result; match = ::nl_cache_get_next(match)) {
		if (::nl_object_match_filter(match, reinterpret_cast< ::nl_object * >(filter))) {
			result = ::rtnl_addr_delete(_socket, reinterpret_cast< ::rtnl_addr * >(match), 0);
		}
	}

	if (result) {
		throw std::runtime_error("Error deleting address: " + boost::lexical_cast<std::string>(result));
	}
}

void rtnl_addr_cache::add(rtnl_addr &addr)
{
	int result = ::rtnl_addr_add(_socket, addr, NLM_F_REPLACE);
	if (result) {
		throw std::runtime_error("Error adding address: " + boost::lexical_cast<std::string>(result));
	}
}

}

// EOF ////////////////////////////////////////////////////////////////////////
