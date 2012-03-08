//==============================================================================
// Brief   : NetworkManager AccessPoint interface implementation
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

#include "AccessPoint.hpp"

AccessPoint::AccessPoint(DBus::Connection &connection, const char* path)
	: DBus::ObjectAdaptor(connection, path)
{
	// FIXME
	Strength = 0;
	MaxBitrate = 0;
	Mode = 0;
	HwAddress = "00:11:22:33:44:55";
	Frequency = 0;
	Ssid = std::vector< uint8_t >();
	RsnFlags = 0;
	WpaFlags = 0;
	Flags = 0;
}

AccessPoint::~AccessPoint()
{
}
