//==============================================================================
// Brief   : NetworkManager MIH User endpoint
// Authors : André Prata <andreprata@av.it.pt>
//------------------------------------------------------------------------------
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

#ifndef NETWORKMANAGER_MIH_USER__HPP_
#define NETWORKMANAGER_MIH_USER__HPP_

#include <odtone/logger.hpp>
#include <odtone/sap/user.hpp>

namespace odtone {
namespace networkmanager {

/**
 * This class provides an implementation of an IEEE 802.21 MIH-User.
 */
class mih_user : boost::noncopyable {
public:
	/**
	 * Construct the MIH-User.
	 *
	 * @param cfg Configuration options.
	 * @param io The io_service object that the MIH-User will use to
	 * dispatch handlers for any asynchronous operations performed on the socket. 
	 */
	mih_user(const odtone::mih::config &cfg, boost::asio::io_service &io);

	/**
	 * Destruct the MIH-User.
	 */
	~mih_user();

protected:
	/**
	 * User registration handler.
	 *
	 * @param cfg Configuration options.
	 * @param ec Error Code.
	 */
	void user_reg_handler(const odtone::mih::config &cfg, const boost::system::error_code &ec);

	/**
	 * Capability Discover handler.
	 *
	 * @param msg Received message.
	 * @param ec Error Code.
	 */
	void capability_discover_confirm(odtone::mih::message& msg, const boost::system::error_code& ec);

	/**
	 * Default MIH event handler.
	 *
	 * @param msg Received message.
	 * @param ec Error code.
	 */
	void event_handler(odtone::mih::message &msg, const boost::system::error_code &ec);

private:
	odtone::sap::user _mihf; /**< User SAP helper. */
	odtone::logger    log_;
};

}; };

#endif /* NETWORKMANAGER_MIH_USER__HPP_ */
