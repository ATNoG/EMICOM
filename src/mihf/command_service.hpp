//==============================================================================
// Brief   : Command Service
// Authors : Simao Reis <sreis@av.it.pt>
//           Carlos Guimarães <cguimaraes@av.it.pt>
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

#ifndef ODTONE_MIHF_COMMAND_SERVICE_HPP
#define ODTONE_MIHF_COMMAND_SERVICE_HPP

///////////////////////////////////////////////////////////////////////////////
#include "link_response_pool.hpp"
#include "local_transaction_pool.hpp"
#include "transmit.hpp"
#include "meta_message.hpp"
#include "link_book.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
///////////////////////////////////////////////////////////////////////////////

namespace odtone { namespace mihf {

/**
 * This class is responsible for handling the messages associated with
 * the command service.
 */
class command_service
	: boost::noncopyable
{
public:
	/**
	 * Construct the command service.
	 *
	 * @param io The io_service object that Link SAP I/O Service will use to
	 * dispatch handlers for any asynchronous operations performed on
	 * the socket.
	 * @param lpool The local transaction pool module.
	 * @param t The transmit module.
	 * @param link_abook The link book module.
	 * @param user_abook The user book module.
	 * @param lrpool The link response pool module.
	 */
	command_service(io_service &io,
	                local_transaction_pool &lpool,
	                transmit &t,
	                link_book &link_abook,
	                user_book &user_abook,
	                link_response_pool &lrpool);

	/**
	 * Link Get Parameters Request message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool link_get_parameters_request(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * Link Get Parameters Response message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool link_get_parameters_response(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * Link Get Parameters Confirm message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool link_get_parameters_confirm(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * Link Configure Thresholds Request message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool link_configure_thresholds_request(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * Link Configure Thresholds Response message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool link_configure_thresholds_response(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * Link Configure Thresholds Confirm message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool link_configure_thresholds_confirm(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * Link Actions Request message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool link_actions_request(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * Link Actions Response message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool link_actions_response(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * Link Actions Confirm message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool link_actions_confirm(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * Net Handover Candidate Query Request message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool net_ho_candidate_query_request(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * Net Handover Candidate Query Response message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool net_ho_candidate_query_response(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * MN Handover Query Request message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool mn_ho_candidate_query_request(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * MN Handover Query Response message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool mn_ho_candidate_query_response(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * N2N Handover Query Resources Request message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool n2n_ho_query_resources_request(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * N2N Handover Query Resources Response message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool n2n_ho_query_resources_response(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * MN Handover Commit Request message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool mn_ho_commit_request(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * MN Handover Commit Response message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool mn_ho_commit_response(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * Net Handover Commit Request message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool net_ho_commit_request(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * Net Handover Commit Response message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool net_ho_commit_response(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * N2N Handover Commit Request message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool n2n_ho_commit_request(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * N2N Handover Commit Response message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool n2n_ho_commit_response(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * MN Handover Complete Request message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool mn_ho_complete_request(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * MN Handover Complete Response message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool mn_ho_complete_response(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * N2N Handover Complete Request message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool n2n_ho_complete_request(meta_message_ptr &in, meta_message_ptr &out);

	/**
	 * N2N Handover Complete Response message handler.
	 *
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool n2n_ho_complete_response(meta_message_ptr &in, meta_message_ptr &out);

private:
	/**
	 * Handler responsible for processing the received Link Get Parameters
	 * responses from Link SAPs.
	 *
	 * @param in The input message.
	 */
	void link_get_parameters_response_handler(meta_message_ptr &in);

	/**
	 * Handler responsible for processing the received Link Action
	 * responses from Link SAPs.
	 *
	 * @param in The input message.
	 */
	void link_actions_response_handler(meta_message_ptr &in);

protected:
	/**
	 * Currently command service handover related messages are handled by
	 * a single MIH-user. If this MIHF is the destination of the message,
	 * forward it to the MIH-User with mobility function.
	 *
	 * @param recv_msg The receive message output.
	 * @param send_msg The send message output.
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool generic_command_request(const char *recv_msg,
				     const char *send_msg,
				     meta_message_ptr &in,
				     meta_message_ptr &out);

	/**
	 * Currently command service handover related messages are handled by
	 * a single MIH-user. If this MIHF is the destination of the message,
	 * check for a pending transaction and forwards the message.
	 *
	 * @param recv_msg The receive message output.
	 * @param send_msg The send message output.
	 * @param in The input message.
	 * @param out The output message.
	 * @return True if the response is sent immediately or false otherwise.
	 */
	bool generic_command_response(const char *recv_msg,
				      const char *send_msg,
				      meta_message_ptr &in,
				      meta_message_ptr &out);

	local_transaction_pool	&_lpool;		/**< Local transaction pool module.	*/
	transmit				&_transmit;		/**< Transmit module.				*/
	link_book				&_link_abook;	/**< Link book module.				*/
	user_book				&_user_abook;	/**< User book module.				*/
	link_response_pool		&_lrpool;		/**< Link response pool module.		*/

	boost::asio::deadline_timer _timer;		/**< Multiple Link SAP requests timer.*/
};

///////////////////////////////////////////////////////////////////////////////
} /* namespace mihf */ } /* namespace odtone */

#endif
