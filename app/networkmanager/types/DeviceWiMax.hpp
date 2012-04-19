//==============================================================================
// Brief   : NetworkManager Device.WiMax interface implementation
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

#ifndef NETWORKMANAGER_DEVICEWIMAX__HPP_
#define NETWORKMANAGER_DEVICEWIMAX__HPP_

#include "../dbus/adaptors/DeviceWiMax.hpp"
#include <boost/noncopyable.hpp>

#include "Device.hpp"

namespace odtone {
namespace networkmanager {

class DeviceWiMax :
	public Device,
	public org::freedesktop::NetworkManager::Device::WiMax_adaptor
{
public:
	DeviceWiMax(DBus::Connection &connection, const char* path);
	~DeviceWiMax();

	// inherited from Device adaptor
	void Disconnect();
	void Enable();

	/**
	 * see Device::link_down()
	 */
	void link_down();

	/**
	 * see Device::link_up()
	 */
	void link_up(const odtone::mih::mac_addr &poa);

	// inherited from WiMax adaptor
	std::vector< ::DBus::Path > GetNspList();
};

}; };

#endif /* NETWORKMANAGER_DEVICEWIMAX__HPP_ */
