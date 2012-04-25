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

#include <odtone/mih/request.hpp>
#include <odtone/mih/response.hpp>
#include <odtone/mih/indication.hpp>
#include <odtone/mih/confirm.hpp>
#include <odtone/mih/tlv_types.hpp>

namespace odtone {
namespace networkmanager {

/**
 * This class provides an implementation of an IEEE 802.21 MIH-User.
 */
class mih_user : boost::noncopyable {
public:
	typedef boost::function<void(mih::message &pm, const boost::system::error_code &ec)> default_handler;
	typedef boost::function<void(mih::network_type &type, mih::link_addr &link)> new_device_handler;

	/**
	 * Construct the MIH-User.
	 *
	 * @param cfg Configuration options.
	 * @param io  The io_service object that the MIH-User will use to dispatch
	 *            handlers for any asynchronous operations performed on the socket.
	 * @param h   The handler for MIHF messages
	 */
	mih_user(const mih::config &cfg, boost::asio::io_service &io, const default_handler &h, const new_device_handler &d);

	/**
	 * Destruct the MIH-User.
	 */
	~mih_user();

protected:
	/**
	 * User registration handler.
	 *
	 * @param ec Error Code.
	 */
	void user_reg_handler(const boost::system::error_code &ec);

	/**
	 * Capability Discover handler.
	 *
	 * @param msg Received message.
	 * @param ec Error Code.
	 */
	void capability_discover_confirm(mih::message& msg, const boost::system::error_code& ec);

	/**
	 * Event subscribe handler.
	 *
	 * @param msg Received message.
	 * @param ec Error Code.
	 */
	void event_subscribe_response(mih::message &msg, const boost::system::error_code &ec);

	/**
	 * Configure thresholds response
	 *
	 * @param msg Received message.
	 * @param ec Error Code.
	 */
	void configure_thresholds_response(mih::message &msg, const boost::system::error_code &ec);

private:
	sap::user _mihf; /**< User SAP helper. */
	logger    log_;

	default_handler    _h;
	new_device_handler _device_handler;
};

}; };

#endif /* NETWORKMANAGER_MIH_USER__HPP_ */
