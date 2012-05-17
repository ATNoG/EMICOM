//==============================================================================
// Brief   : NetworkManager Device.Modem interface implementation
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

#ifndef NETWORKMANAGER_DEVICEWIRED__HPP_
#define NETWORKMANAGER_DEVICEWIRED__HPP_

#include "../dbus/adaptors/DeviceModem.hpp"
#include <boost/noncopyable.hpp>

#include "Device.hpp"

namespace odtone {
namespace networkmanager {

class DeviceModem :
	public Device,
	public org::freedesktop::NetworkManager::Device::Modem_adaptor
{
public:
	/**
	 * Construct a new 3g Device interface.
	 *
	 * @param connection The D-Bus connection to serve the interface at.
	 * @param path       The D-Bus path for the object.
	 * @param ctrl       The MIH User interface to control the device
	 * @param lti        A tuple that identifies this interface.
	 */
	DeviceModem(DBus::Connection &connection, const char* path, mih_user &ctrl, mih::link_tuple_id &lti);
	~DeviceModem();

	/**
	 * see Device::link_down()
	 */
	void link_down();

	/**
	 * see Device::link_up()
	 */
	void link_up(const boost::optional<mih::mac_addr> &poa);
};

}; };

#endif /* NETWORKMANAGER_DEVICEWIRED__HPP_ */
