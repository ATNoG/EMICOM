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

#ifndef __NLWRAP_NL_ADDR_
#define __NLWRAP_NL_ADDR_

#include <netlink/addr.h>

namespace nlwrap {

/**
 * This class provides a RAI wrapper for the nl_addr datatype
 */
class nl_addr {

public:
	enum ADDRESS_FAMILY {
		ADDRESS_FAMILY_AF_INET   = AF_INET,
		ADDRESS_FAMILY_AF_INET6  = AF_INET6
	};

public:

	/**
	 * Construct a new nl_addr object.
	 * Automatically allocates the underlying nl_addr struct.
	 *
	 * @param fam  The family of the address.
	 * @param raw  The raw bytes of the address.
	 * @param size The number of bytes of the address.
	 */
	nl_addr(ADDRESS_FAMILY fam, void *raw, unsigned int size);

	/**
	 * Construct a new nl_addr object.
	 * Automatically allocates the underlying nl_addr struct.
	 *
	 * @param fam       The family of the address.
	 * @param raw       The raw bytes of the address.
	 * @param size      The number of bytes of the address.
	 * @param prefixlen The prefix length of the address.
	 */
	nl_addr(ADDRESS_FAMILY fam, void *raw, unsigned int size, int prefixlen);

	/**
	 * Construct an nl_addr object from a preallocated nl_addr struct.
	 *
	 * @warning This does not deallocate the object upon destruction.
	 *
	 * @param addr The preallocated nl_addr struct.
	 */
	nl_addr(::nl_addr *addr);

	/**
	 * Copy constructor from another nl_addr object.
	 *
	 * @warning Unlike nl_addr(::nl_addr *),
	 *          this effectively copies the object,
	 *          and deallocates it on destruction.
	 *
	 * @param copy The object to copy.
	 */
	explicit nl_addr(const nl_addr &copy);

	/**
	 * Destruct the object. Frees the underlying nl_addr struct, if allocated on this obect's context.
	 */
	~nl_addr();

	/**
	 * Allow direct usage of the underlying nl_addr pointer.
	 */
	operator ::nl_addr *();

	/**
	 * Similar to constructing from a nl_addr struct pointer.
	 *
	 * @param addr The struct pointer to construct from.
	 */
	nl_addr &operator =(::nl_addr *addr);

	/**
	 * Set the prefix length of this nl_addr.
	 *
	 * @param len The prefix length.
	 */
	void set_prefixlen(int len);

private:
	bool       _own;
	::nl_addr *_addr;
};

}

// EOF ////////////////////////////////////////////////////////////////////////

#endif /* __NLWRAP_NL_ADDR_ */
