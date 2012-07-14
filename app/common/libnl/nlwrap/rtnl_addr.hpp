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

#ifndef __NLWRAP_RTNL_ADDR_
#define __NLWRAP_RTNL_ADDR_

#include <netlink/route/addr.h>
#include "nl_addr.hpp"
#include <boost/noncopyable.hpp>

namespace nlwrap {

/**
 * This class provides a RAI wrapper for the rtnl_addr datatype
 */
class rtnl_addr : boost::noncopyable {

public:
	enum ADDRESS_SCOPE {
		ADDRESS_SCOPE_GLOBAL  = 0,
		ADDRESS_SCOPE_SITE    = 200,
		ADDRESS_SCOPE_LINK    = 253,
		ADDRESS_SCOPE_HOST    = 254,
		ADDRESS_SCOPE_NOWHERE = 255
	};

public:

	/**
	 * Construct a new rtnl_addr object.
	 * Automatically allocates the underlying rtnl_addr struct.
	 */
	rtnl_addr();

	/**
	 * Construct an rtnl_addr object from a preallocated rtnl_addr struct.
	 *
	 * @warning This does not deallocate the object upon destruction.
	 *
	 * @param addr The preallocated rtnl_addr struct.
	 */
	rtnl_addr(::rtnl_addr *addr);

	/**
	 * Destruct the object. Frees the underlying rtnl_addr struct, if allocated on this obect's context.
	 */
	~rtnl_addr();

	/**
	 * Allow direct usage of the underlying rtnl_addr pointer.
	 */
	operator ::rtnl_addr *();

	/**
	 * Similar to constructing from a rtnl_addr struct pointer.
	 *
	 * @param addr The struct pointer to construct from.
	 */
	rtnl_addr &operator =(::rtnl_addr *addr);

	/**
	 * Set the interface index of this rtnl_addr.
	 *
	 * @param ifindex The interface index.
	 */
	void set_ifindex(int ifindex);

	/**
	 * Set the family for this address.
	 *
	 * @param fam The family.
	 */
	void set_family(nl_addr::ADDRESS_FAMILY fam);

	/**
	 * Set the local address of this rtnl_addr.
	 *
	 * @param addr The local address.
	 */
	void set_local(nl_addr &addr);

	/**
	 * Set the local address of this rtnl_addr.
	 *
	 * @param addr The local address.
	 */
	void set_peer(nl_addr &addr);

	/**
	 * Set the prefix length of this rtnl_addr.
	 *
	 * @param len The prefix length.
	 */
	void set_prefixlen(int len);

	/**
	 * Set the broadcast address of this rtnl_addr.
	 *
	 * @param addr The broadcast address.
	 */
	void set_broadcast(nl_addr &addr);

	/**
	 * Set the multicast address of this rtnl_addr.
	 *
	 * @param addr The multicast address.
	 */
	void set_multicast(nl_addr &addr);

	/**
	 * Set the anycast address of this rtnl_addr.
	 *
	 * @param addr The anycast address.
	 */
	void set_anycast(nl_addr &addr);

	/**
	 * Set the scope of this rtnl_addr.
	 *
	 * @param sc The scope.
	 */
	void set_scope(ADDRESS_SCOPE sc);

private:
	bool         _own;
	::rtnl_addr *_addr;
};

}

// EOF ////////////////////////////////////////////////////////////////////////

#endif /* __NLWRAP_RTNL_ADDR_ */
