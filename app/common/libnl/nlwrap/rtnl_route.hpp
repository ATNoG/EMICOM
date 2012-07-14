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

#ifndef __NLWRAP_RTNL_ROUTE_
#define __NLWRAP_RTNL_ROUTE_

#include <netlink/route/route.h>
#include "nl_addr.hpp"
#include <boost/noncopyable.hpp>

namespace nlwrap {

/**
 * This class provides a RAI wrapper for the rtnl_route datatype
 */
class rtnl_route : boost::noncopyable {

public:
	enum ROUTE_SCOPE {
		ROUTE_SCOPE_UNIVERSE = 0,
		ROUTE_SCOPE_SITE     = 200,
		ROUTE_SCOPE_LINK     = 253,
		ROUTE_SCOPE_HOST     = 254,
		ROUTE_SCOPE_NOWHERE  = 255
	};

	enum ROUTE_FAMILY {
		ROUTE_FAMILY_AF_INET  = AF_INET,
		ROUTE_FAMILY_AF_INET6 = AF_INET6
	};

	enum ROUTE_TABLE {
		ROUTE_TABLE_UNSPEC = 0,
		ROUTE_TABLE_COMPAT = 252,
		ROUTE_TABLE_DEFAULT = 253,
		ROUTE_TABLE_MAIN = 254,
		ROUTE_TABLE_LOCAL = 255,
	};

public:

	/**
	 * Construct a new rtnl_route object.
	 * Automatically allocates the underlying rtnl_route struct.
	 */
	rtnl_route();

	/**
	 * Construct a new rtnl_route object with some parameters.
	 *
	 * @param ifindex @see rtnl_route::set_oifindex().
	 * @param dst     @see rtnl_route::set_dst().
	 * @param gateway @see rtnl_route::set_gateway().
	 * @param scope   @see rtnl_route::set_scope().
	 * @param fam     @see rtnl_route::set_fam().
	 */
	rtnl_route(int ifindex, nl_addr &dst, nl_addr &gateway, ROUTE_SCOPE scope, ROUTE_FAMILY fam);

	/**
	 * Construct an rtnl_route object from a preallocated rtnl_route struct.
	 *
	 * @warning This does not deallocate the object upon destruction.
	 *
	 * @param addr The preallocated rtnl_route struct.
	 */
	rtnl_route(::rtnl_route *route);

	/**
	 * Destruct the object. Frees the underlying rtnl_route struct, if allocated on this obect's context.
	 */
	~rtnl_route();

	/**
	 * Allow direct usage of the underlying rtnl_route pointer.
	 */
	operator ::rtnl_route *();

	/**
	 * Similar to constructing from a rtnl_route struct pointer.
	 *
	 * @param addr The struct pointer to construct from.
	 */
	rtnl_route &operator =(::rtnl_route *route);

	/**
	 * Set the output interface index of this rtnl_route.
	 *
	 * @param ifindex The interface index.
	 */
	void set_oifindex(int ifindex);

	/**
	 * Set the destination network for this route.
	 *
	 * @param addr The network.
	 */
	void set_dst(nl_addr &addr);

	/**
	 * Set the next hop gateway for this route.
	 *
	 * @warning Set the oifindex() before this!
	 *
	 * @param addr The next hop address.
	 */
	void set_gateway(nl_addr &addr);

	/**
	 * Set the scope for this route.
	 *
	 * @param scope The scope.
	 */
	void set_scope(ROUTE_SCOPE scope);

	/**
	 * Set the family for this route.
	 *
	 * @param fam The family.
	 */
	void set_family(ROUTE_FAMILY fam);

	/**
	 * Set the table for this route.
	 *
	 * @param table The table.
	 */
	void set_table(ROUTE_TABLE table);

private:
	bool          _own;
	::rtnl_route *_route;
};

}

// EOF ////////////////////////////////////////////////////////////////////////

#endif /* __NLWRAP_RTNL_ADDR_ */
