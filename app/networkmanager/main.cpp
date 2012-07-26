//==============================================================================
// Brief   : NetworkManager main entry point
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

#include "types/types.hpp"
#include "mih_user.hpp"

#include <odtone/logger.hpp>

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <cstdlib> // for EXIT_{SUCCESS,FAILURE}
#include <iostream>

#include <boost/thread/mutex.hpp>

using namespace odtone;
namespace po = boost::program_options;
namespace nm = odtone::networkmanager;

void dbus_dispatch_pending(DBus::BusDispatcher &d, boost::mutex &m)
{
	d.dispatch_pending();
	m.unlock();
}

int main(int argc, char *argv[])
{
	odtone::setup_crash_handler();

	// setup mih launch configurations
	po::options_description desc(odtone::mih::octet_string("MIH Usr Configuration"));
	desc.add_options()
		("help", "Display configuration options")
		(sap::kConf_File, po::value<std::string>()->default_value("networkmanager.conf"), "Configuration file")
		(sap::kConf_Receive_Buffer_Len, po::value<uint>()->default_value(4096), "Receive buffer length")
		(sap::kConf_Port, po::value<ushort>()->default_value(1234), "Listening port")
		(sap::kConf_MIH_SAP_id, po::value<std::string>()->default_value("mih_nm"), "MIH-User ID")
		(sap::kConf_MIHF_Ip, po::value<std::string>()->default_value("127.0.0.1"), "Local MIHF IP address")
		(sap::kConf_MIHF_Local_Port, po::value<ushort>()->default_value(1025), "Local MIHF communication port")
		(sap::kConf_MIH_SAP_dest, po::value<std::string>()->default_value(""), "MIHF destination")
		(nm::kConf_DBus_Name, po::value<std::string>()->default_value("org.freedesktop.NetworkManager"),
		                  "DBus name for NetworkManager bus")
		(nm::kConf_DBus_Path, po::value<std::string>()->default_value("/org/freedesktop/NetworkManager21"),
		                  "DBus path for NetworkManager object")
		(nm::kConf_Settings_Path, po::value<std::string>()->default_value("./settings"),
		                      "System path for NetworkManager connection persistence")
		(nm::kConf_Version, po::value<std::string>()->default_value("0.9.4.0"),
		                      "NetworkManager version to mimic");

	odtone::mih::config cfg(desc);
	cfg.parse(argc, argv, odtone::sap::kConf_File);

	if (cfg.help()) {
		std::cerr << desc << std::endl;
		return EXIT_SUCCESS;
	}

	// setup D-Bus service
	DBus::BusDispatcher dispatcher;
	DBus::default_dispatcher = &dispatcher;

	DBus::Connection conn = DBus::Connection::SystemBus();
	conn.request_name(cfg.get<std::string>(nm::kConf_DBus_Name).c_str());

	// launch the service
	boost::asio::io_service ios;
	nm::NetworkManager manager(conn, cfg, ios);

	boost::thread io(boost::bind(&boost::asio::io_service::run, &ios));

	boost::mutex m;
	while (true) {
		ios.dispatch(boost::bind(&dbus_dispatch_pending, boost::ref(dispatcher), boost::ref(m)));
		m.lock();
		dispatcher.dispatch();
	}

	return EXIT_SUCCESS;
}
