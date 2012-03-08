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

#ifndef DEVICEWIMAX__HPP_
#define DEVICEWIMAX__HPP_

#include "../adaptors/Device.hpp"
#include "../adaptors/DeviceWiMax.hpp"

class DeviceWiMax :
	public org::freedesktop::NetworkManager::Device_adaptor,
	public org::freedesktop::NetworkManager::Device::WiMax_adaptor,
	public DBus::IntrospectableAdaptor,
	public DBus::PropertiesAdaptor,
	public DBus::ObjectAdaptor
{
public:
	DeviceWiMax(DBus::Connection &connection, const char* path);
	~DeviceWiMax();

	// inherited from Device adaptor
	void Disconnect();

	// inherited from WiMax adaptor
	std::vector< ::DBus::Path > GetNspList();
};

#endif /* DEVICEWIMAX__HPP_ */
