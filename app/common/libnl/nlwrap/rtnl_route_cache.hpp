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

#ifndef __NLWRAP_RTNL_ROUTE_CACHE_
#define __NLWRAP_RTNL_ROUTE_CACHE_

#include <boost/noncopyable.hpp>

#include "rtnl_socket.hpp"
#include "rtnl_route.hpp"

namespace nlwrap {

/**
 * This class provides a RAI wrapper for the rtnl_route_cache datatype
 */
class rtnl_route_cache : boost::noncopyable {

public:
	/**
	 * Construct a new rtnl_route_cache object.
	 * Automatically allocates a struct counterpart.
	 */
	rtnl_route_cache();

	/**
	 * Destruct the object. Frees the underlying struct.
	 */
	~rtnl_route_cache();

	/**
	 * Clear routes from the system.
	 *
	 * @param route The route to remove.
	 */
	void clear(rtnl_route &route);

	/**
	 * Add a new route to the system.
	 *
	 * @warning The "replace" flag is always set.
	 *
	 * @param route The route.
	 */
	void add(rtnl_route &route);

private:
	rtnl_socket  _socket;
	::nl_cache  *_cache;
};

}

// EOF ////////////////////////////////////////////////////////////////////////

#endif /* __NLWRAP_RTNL_ROUTE_CACHE_ */
