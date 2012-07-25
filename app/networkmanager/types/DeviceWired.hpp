//==============================================================================
// Brief   : NetworkManager Device.Wired interface implementation
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

#include "../dbus/adaptors/DeviceWired.hpp"

#include "Device.hpp"

namespace odtone {
namespace networkmanager {

class DeviceWired :
	public Device,
	public org::freedesktop::NetworkManager::Device::Wired_adaptor
{
public:
	/**
	 * Construct a new ethernet Device interface.
	 *
	 * @param connection @see odtone::networkmanager::Device::Device()
	 * @param path       @see odtone::networkmanager::Device::Device()
	 * @param ctrl       @see odtone::networkmanager::Device::Device()
	 * @param lti        @see odtone::networkmanager::Device::Device()
	 */
	DeviceWired(DBus::Connection &connection, const char* path, mih_user &ctrl, mih::link_tuple_id &lti);
	~DeviceWired();

	/**
	 * @see Device::Disable()
	 */
	void Disable();

	/**
	 * @see Device::Disconnect()
	 */
	void Disconnect();

	/**
	 * @see Device::link_conf()
	 */
	virtual void link_conf(const completion_handler &h,
	                       const boost::optional<mih::link_addr> &poa,
	                       const mih::configuration_list &lconf,
	                       const DBus::Path &connection_active,
	                       const DBus::Path &specific_object);

protected:

	/**
	 * @see Device::state()
	 */
	void state(NM_DEVICE_STATE newstate, NM_DEVICE_STATE_REASON reason);
};

}; };

#endif /* NETWORKMANAGER_DEVICEWIRED__HPP_ */
