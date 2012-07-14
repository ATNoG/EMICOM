//==============================================================================
// Brief   : D-Bus dispatcher using Boost::Asio
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

#ifndef __ODTONE_DBUS_ASIO_DISPATCHER_
#define __ODTONE_DBUS_ASIO_DISPATCHER_

#include <dbus-c++/dbus.h>
#include <boost/asio.hpp>
#include <boost/thread/shared_mutex.hpp>

namespace odtone {
namespace dbus {

class asio_dispatcher :
	public DBus::Dispatcher,
	public DBus::DefaultMainLoop
{
public:
	asio_dispatcher(boost::asio::io_service& io);

	~asio_dispatcher();

	void enter();

	void leave();

	DBus::Timeout *add_timeout(DBus::Timeout::Internal *);

	void rem_timeout(DBus::Timeout *);

	DBus::Watch *add_watch(DBus::Watch::Internal *);

	void rem_watch(DBus::Watch *);

private:
	void do_iteration();

	void watch_ready(DBus::DefaultWatch &);

	void timeout_expired(DBus::DefaultTimeout &);

private:
	boost::asio::io_service &_ios;
	bool                     _running;
	boost::shared_mutex      _mutex;
};

class asio_timeout :
	public DBus::Timeout,
	public DBus::DefaultTimeout
{
	asio_timeout(DBus::Timeout::Internal *, asio_dispatcher *);

	void toggle();

	friend class asio_dispatcher;
};

class asio_watch :
	public DBus::Watch,
	public DBus::DefaultWatch
{
	asio_watch(DBus::Watch::Internal *, asio_dispatcher *);

	void toggle();

	friend class asio_dispatcher;
};

}; };

#endif /* __ODTONE_DBUS_ASIO_DISPATCHER_ */

