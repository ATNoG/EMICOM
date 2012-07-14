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

#ifndef __NLWRAP_RTNL_ADDR_CACHE_
#define __NLWRAP_RTNL_ADDR_CACHE_

#include <boost/noncopyable.hpp>

#include "rtnl_socket.hpp"
#include "rtnl_addr.hpp"

namespace nlwrap {

/**
 * This class provides a RAI wrapper for the rtnl_addr_cache datatype
 */
class rtnl_addr_cache : boost::noncopyable {

public:
	/**
	 * Construct a new rtnl_addr_cache object.
	 * Automatically allocates a struct counterpart.
	 */
	rtnl_addr_cache();

	/**
	 * Destruct the object. Frees the underlying struct.
	 */
	~rtnl_addr_cache();

	/**
	 * Clear addresses on the system.
	 *
	 * @note The only two required parameters are
	 *       the interface index and
	 *       the address family.
	 *
	 * @warning This method ignores error ENODEV!
	 *          (issued on if address does not exist)
	 *
	 * @param addr The address to delete.
	 */
	void clear(rtnl_addr &addr);

	/**
	 * Add an address to a link.
	 *
	 * @param addr the address.
	 */
	void add(rtnl_addr &addr);

private:
	rtnl_socket  _socket;
	::nl_cache  *_cache;
};

}

// EOF ////////////////////////////////////////////////////////////////////////

#endif /* __NLWRAP_RTNL_ADDR_CACHE_ */
