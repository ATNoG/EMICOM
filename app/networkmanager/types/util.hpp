//==============================================================================
// Brief   : NetworkManager Device specific types
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

#ifndef NETWORKMANAGER_UTIL__HPP_
#define NETWORKMANAGER_UTIL__HPP_

namespace odtone {
namespace networkmanager {

	/**
	 * Convert a given value to the Variant type.
	 *
	 * @param value The value to convert to Variant.
	 * @return The result Variant value.
	 */
	template <class T>
	DBus::Variant to_variant(const T &value)
	{
		DBus::Variant v;
		DBus::MessageIter iter = v.writer();
		iter << value;
		return v;
	}

	template <class T>
	T from_variant(const DBus::Variant &v)
	{
		T value;
		DBus::MessageIter iter = v.reader();
		iter >> value;
		return value;
	}

}; };

#endif /* NETWORKMANAGER_UTIL__HPP_ */
