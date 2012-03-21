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

#include "mih_user.hpp"

#include <odtone/mih/request.hpp>
#include <odtone/mih/response.hpp>
#include <odtone/mih/indication.hpp>
#include <odtone/mih/confirm.hpp>
#include <odtone/mih/tlv_types.hpp>

#include <boost/bind.hpp>
#include <iostream>

mih_user::mih_user(const odtone::mih::config &cfg, boost::asio::io_service &io) :
	_mihf(cfg, io, boost::bind(&mih_user::event_handler, this, _1, _2)),
	log_("mih_usr", std::cout)
{
	odtone::mih::message m;

	boost::optional<odtone::mih::mih_cmd_list> supp_cmd;

	m << odtone::mih::indication(odtone::mih::indication::user_register)
	    & odtone::mih::tlv_command_list(supp_cmd);
	m.destination(odtone::mih::id("local-mihf"));

	_mihf.async_send(m, boost::bind(&mih_user::user_reg_handler, this, boost::cref(cfg), _2));
}

mih_user::~mih_user()
{
}

void mih_user::user_reg_handler(const odtone::mih::config &cfg, const boost::system::error_code &ec)
{
	log_(0, "MIH-User registered, status: ", ec.message());
}

void mih_user::event_handler(odtone::mih::message &msg, const boost::system::error_code &ec)
{
	log_(0, "Event received, status: ", ec.message());
}
