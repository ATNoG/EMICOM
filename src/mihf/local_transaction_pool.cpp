//==============================================================================
// Brief   : Local Transaction Pool
// Authors : Simao Reis <sreis@av.it.pt>
//------------------------------------------------------------------------------
// ODTONE - Open Dot Twenty One
//
// Copyright (C) 2009-2011 Universidade Aveiro
// Copyright (C) 2009-2011 Instituto de Telecomunicações - Pólo Aveiro
//
// This software is distributed under a license. The full license
// agreement can be found in the file LICENSE in this distribution.
// This software may not be copied, modified, sold or distributed
// other than expressed in the named license agreement.
//
// This software is distributed without any warranty.
//==============================================================================

///////////////////////////////////////////////////////////////////////////////
#include "local_transaction_pool.hpp"
#include "log.hpp"
///////////////////////////////////////////////////////////////////////////////

namespace odtone { namespace mihf {

/**
 * Construct a Local Transaction Pool.
 */
local_transaction_pool::local_transaction_pool()
{
}

/**
 * Add a new entry in the Local Transaction Pool.
 *
 * @param in The input message.
 */
void local_transaction_pool::add(meta_message_ptr& in)
{
	pending_transaction p;

	p.user.assign(in->source().to_string());
	p.destination.assign(in->destination().to_string());
	p.tid = in->tid();

	ODTONE_LOG(3, "(local transactions) added transaction ", p.user, ":",
	    p.destination, ":", p.tid);

	{
		boost::mutex::scoped_lock lock(_mutex);
		_transactions.push_back(p);
	}
}

/**
 * Remove an existing entry from the Local Transaction Pool.
 *
 * @param id The MIH source identifier.
 * @param tid The transaction identifier.
 */
void local_transaction_pool::del(const mih::octet_string user,
                                 uint16 tid)
{
	std::list<pending_transaction>::iterator it;
	it = find(user);

	if (it != _transactions.end()) {
		if(it->tid == tid) {
			boost::mutex::scoped_lock lock(_mutex);
			_transactions.erase(it);
		}
	}
}

/**
 * Searchs for a record in the Local Transaction Pool.
 *
 * @param from The MIH source identifier.
 * @return The list of active transaction from the given source.
 */
std::list<pending_transaction>::iterator
local_transaction_pool::find(const mih::octet_string &from)
{
	std::list<pending_transaction>::iterator it;

	for(it = _transactions.begin(); it != _transactions.end(); it++) {
		if (it->destination == from)
			break;
	}
	return it;
}

/**
 * Check the existence of an active transaction. It also set the
 * MIH destination and the transaction identifier in the given message.
 *
 * @param msg The MIH Message.
 * @return True if there is an active transaction or false otherwise.
 */
bool local_transaction_pool::set_user_tid(meta_message_ptr &msg)
{
	boost::mutex::scoped_lock lock(_mutex);

	std::list<pending_transaction>::iterator it;
	it = find(msg->source().to_string());

	if (it != _transactions.end()) {

		msg->tid(it->tid);
		msg->destination(mih::id(it->user));

		_transactions.erase(it);
		return true;
	}

	return false;
}

} /* namespace mihf */ } /* namespace odtone */
