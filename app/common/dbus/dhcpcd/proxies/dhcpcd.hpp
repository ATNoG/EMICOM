//==============================================================================
// Brief   : dhcpcd main D-Bus interface proxy
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

#ifndef __dbusxx_____proxies_dhcpcd_hpp__PROXY_MARSHAL_H
#define __dbusxx_____proxies_dhcpcd_hpp__PROXY_MARSHAL_H

#include <dbus-c++/dbus.h>
#include <cassert>

namespace name {
namespace marples {
namespace roy {

class dhcpcd_proxy
: public ::DBus::InterfaceProxy
{
public:

    dhcpcd_proxy()
    : ::DBus::InterfaceProxy("name.marples.roy.dhcpcd")
    {
        connect_signal(dhcpcd_proxy, Event, _Event_stub);
        connect_signal(dhcpcd_proxy, StatusChanged, _StatusChanged_stub);
        connect_signal(dhcpcd_proxy, ScanResultsSignal, _ScanResultsSignal_stub);
    }

public:

    /* properties exported by this interface */
public:

    /* methods exported by this interface,
     * this functions will invoke the corresponding methods on the remote objects
     */
    std::string GetVersion()
    {
        ::DBus::CallMessage call;
        call.member("GetVersion");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        std::string argout;
        ri >> argout;
        return argout;
    }

    std::string GetDhcpcdVersion()
    {
        ::DBus::CallMessage call;
        call.member("GetDhcpcdVersion");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        std::string argout;
        ri >> argout;
        return argout;
    }

    std::vector< std::string > ListInterfaces()
    {
        ::DBus::CallMessage call;
        call.member("ListInterfaces");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        std::vector< std::string > argout;
        ri >> argout;
        return argout;
    }

    std::map< std::string, std::map< std::string, ::DBus::Variant > > GetInterfaces()
    {
        ::DBus::CallMessage call;
        call.member("GetInterfaces");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        std::map< std::string, std::map< std::string, ::DBus::Variant > > argout;
        ri >> argout;
        return argout;
    }

    std::string GetStatus()
    {
        ::DBus::CallMessage call;
        call.member("GetStatus");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        std::string argout;
        ri >> argout;
        return argout;
    }

    void Rebind(const std::string& interface)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << interface;
        call.member("Rebind");
        ::DBus::Message ret = invoke_method (call);
    }

    void Release(const std::string& interface)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << interface;
        call.member("Release");
        ::DBus::Message ret = invoke_method (call);
    }

    void Stop(const std::string& interface)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << interface;
        call.member("Stop");
        ::DBus::Message ret = invoke_method (call);
    }

    std::vector< std::string > GetConfigBlocks(const std::string& block)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << block;
        call.member("GetConfigBlocks");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        std::vector< std::string > argout;
        ri >> argout;
        return argout;
    }

    std::vector< std::vector< ::DBus::Struct< std::string, std::string > > > GetConfig(const std::string& block, const std::string& name)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << block;
        wi << name;
        call.member("GetConfig");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        std::vector< std::vector< ::DBus::Struct< std::string, std::string > > > argout;
        ri >> argout;
        return argout;
    }

    void SetConfig(const std::string& block, const std::string& name, const std::vector< std::vector< ::DBus::Struct< std::string, std::string > > >& config)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << block;
        wi << name;
        wi << config;
        call.member("SetConfig");
        ::DBus::Message ret = invoke_method (call);
    }

    void Scan(const std::string& interface)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << interface;
        call.member("Scan");
        ::DBus::Message ret = invoke_method (call);
    }

    std::vector< ::DBus::Struct< std::map< std::string, ::DBus::Variant > > > ScanResults(const std::string& interface)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << interface;
        call.member("ScanResults");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        std::vector< ::DBus::Struct< std::map< std::string, ::DBus::Variant > > > argout;
        ri >> argout;
        return argout;
    }

    std::vector< std::vector< ::DBus::Struct< int32_t, std::string, std::string, std::string > > > ListNetworks(const std::string& interface)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << interface;
        call.member("ListNetworks");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        std::vector< std::vector< ::DBus::Struct< int32_t, std::string, std::string, std::string > > > argout;
        ri >> argout;
        return argout;
    }

    int32_t AddNetwork(const std::string& interface)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << interface;
        call.member("AddNetwork");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        int32_t argout;
        ri >> argout;
        return argout;
    }

    void RemoveNetwork(const std::string& interface, const int32_t& id)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << interface;
        wi << id;
        call.member("RemoveNetwork");
        ::DBus::Message ret = invoke_method (call);
    }

    void EnableNetwork(const std::string& interface, const int32_t& id)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << interface;
        wi << id;
        call.member("EnableNetwork");
        ::DBus::Message ret = invoke_method (call);
    }

    void DisableNetwork(const std::string& interface, const int32_t& id)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << interface;
        wi << id;
        call.member("DisableNetwork");
        ::DBus::Message ret = invoke_method (call);
    }

    void SelectNetwork(const std::string& interface, const int32_t& id)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << interface;
        wi << id;
        call.member("SelectNetwork");
        ::DBus::Message ret = invoke_method (call);
    }

    std::string GetNetwork(const std::string& interface, const int32_t& id, const std::string& parameter)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << interface;
        wi << id;
        wi << parameter;
        call.member("GetNetwork");
        ::DBus::Message ret = invoke_method (call);
        ::DBus::MessageIter ri = ret.reader();

        std::string argout;
        ri >> argout;
        return argout;
    }

    void SetNetwork(const std::string& interface, const int32_t& id, const std::string& parameter, const std::string& value)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << interface;
        wi << id;
        wi << parameter;
        wi << value;
        call.member("SetNetwork");
        ::DBus::Message ret = invoke_method (call);
    }

    void SaveConfig(const std::string& interface)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << interface;
        call.member("SaveConfig");
        ::DBus::Message ret = invoke_method (call);
    }

    void Disconnect(const std::string& interface)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << interface;
        call.member("Disconnect");
        ::DBus::Message ret = invoke_method (call);
    }

    void Reassociate(const std::string& interface)
    {
        ::DBus::CallMessage call;
        ::DBus::MessageIter wi = call.writer();

        wi << interface;
        call.member("Reassociate");
        ::DBus::Message ret = invoke_method (call);
    }


public:

    /* signal handlers for this interface
     */
    virtual void Event(const std::map< std::string, ::DBus::Variant >& configuration) = 0;
    virtual void StatusChanged(const std::string& status) = 0;
    virtual void ScanResultsSignal(const std::string& interface) = 0;

private:

    /* unmarshalers (to unpack the DBus message before calling the actual signal handler)
     */
    void _Event_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        std::map< std::string, ::DBus::Variant > configuration;
        ri >> configuration;
        Event(configuration);
    }
    void _StatusChanged_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        std::string status;
        ri >> status;
        StatusChanged(status);
    }
    void _ScanResultsSignal_stub(const ::DBus::SignalMessage &sig)
    {
        ::DBus::MessageIter ri = sig.reader();

        std::string interface;
        ri >> interface;
        ScanResultsSignal(interface);
    }
};

} } }
#endif //__dbusxx_____proxies_dhcpcd_hpp__PROXY_MARSHAL_H
