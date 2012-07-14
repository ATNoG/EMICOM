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

#include "asio_dispatcher.hpp"

#include <boost/bind.hpp>
#define DBUS_WATCH_READABLE (1 << 0)
#define DBUS_WATCH_WRITABLE (1 << 1)
#define DBUS_WATCH_ERROR    (1 << 2)
#define DBUS_WATCH_HANGUP   (1 << 3)

using namespace odtone::dbus;

asio_dispatcher::asio_dispatcher(boost::asio::io_service& io) : _ios(io), _running(false)
{
}

asio_dispatcher::~asio_dispatcher()
{
}

void asio_dispatcher::enter()
{
	boost::unique_lock<boost::shared_mutex> lock(_mutex);
	_running = true;

	//do_iteration();
	_ios.post(boost::bind(&asio_dispatcher::do_iteration, this));
}

void asio_dispatcher::do_iteration()
{
	dispatch_pending();
	dispatch();

	if (_running) {
		_ios.dispatch(boost::bind(&asio_dispatcher::enter, this));
	}
}

void asio_dispatcher::leave()
{
	boost::unique_lock<boost::shared_mutex> lock(_mutex);
	_running = false;
}

DBus::Timeout *asio_dispatcher::add_timeout(DBus::Timeout::Internal *ti)
{
	asio_timeout *bt = new asio_timeout(ti, this);

	bt->expired = new DBus::Callback<asio_dispatcher, void, DBus::DefaultTimeout &>(this, &asio_dispatcher::timeout_expired);
	bt->data(bt);

	return bt;
}

void asio_dispatcher::rem_timeout(DBus::Timeout *t)
{
	delete t;
}

DBus::Watch *asio_dispatcher::add_watch(DBus::Watch::Internal *wi)
{
	asio_watch *bw = new asio_watch(wi, this);

	bw->ready = new DBus::Callback<asio_dispatcher, void, DBus::DefaultWatch &>(this, &asio_dispatcher::watch_ready);
	bw->data(bw);

	return bw;
}

void asio_dispatcher::rem_watch(DBus::Watch *w)
{
	delete w;
}

void asio_dispatcher::watch_ready(DBus::DefaultWatch &ew)
{
	asio_watch *watch = reinterpret_cast<asio_watch *>(ew.data());

	int flags = 0;
	if (watch->state() & POLLIN) {
		flags |= DBUS_WATCH_READABLE;
	}
	if (watch->state() & POLLOUT) {
		flags |= DBUS_WATCH_WRITABLE;
	}
	if (watch->state() & POLLHUP) {
		flags |= DBUS_WATCH_HANGUP;
	}
	if (watch->state() & POLLERR) {
		flags |= DBUS_WATCH_ERROR;
	}

	watch->handle(flags);
}

void asio_dispatcher::timeout_expired(DBus::DefaultTimeout &et)
{
	asio_timeout *timeout = reinterpret_cast<asio_timeout *>(et.data());

	timeout->handle();
}


asio_timeout::asio_timeout(DBus::Timeout::Internal *ti, asio_dispatcher *bd) :
	DBus::Timeout(ti), DBus::DefaultTimeout(Timeout::interval(), true, bd)
{
	DBus::DefaultTimeout::enabled(DBus::Timeout::enabled());
}

void asio_timeout::toggle()
{
	DBus::DefaultTimeout::enabled(DBus::Timeout::enabled());
}


asio_watch::asio_watch(DBus::Watch::Internal *wi, asio_dispatcher *bd) :
	DBus::Watch(wi), DBus::DefaultWatch(Watch::descriptor(), 0, bd)
{
	int flags = POLLHUP | POLLERR;

	if (Watch::flags() & DBUS_WATCH_READABLE)
		flags |= POLLIN;
	if (Watch::flags() & DBUS_WATCH_WRITABLE)
		flags |= POLLOUT;

	DBus::DefaultWatch::flags(flags);
	DBus::DefaultWatch::enabled(DBus::Watch::enabled());
}

void asio_watch::toggle()
{
	DBus::DefaultWatch::enabled(DBus::Watch::enabled());
}
