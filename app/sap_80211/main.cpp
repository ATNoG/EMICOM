//=============================================================================
// Brief   : 80211 Link SAP entry point
// Authors : André Prata <andreprata@av.it.pt>
//-----------------------------------------------------------------------------
// ODTONE - Open Dot Twenty One
//
// Copyright (C) 2009-2011 Universidade Aveiro
// Copyright (C) 2009-2011 Instituto de Telecomunicações - Pólo Aveiro
//
// This software is distributed under a license. The full license
// agreement can be found in the file LICENSE in this distribution.
// This software may not be copied, modified, sold or distributed
// other than expressed in the named license agreement.
//
// This software is distributed without any warranty.
//==============================================================================

///////////////////////////////////////////////////////////////////////////////

#include <unistd.h> // for geteuid

#include <odtone/sap/link.hpp>
#include <odtone/mih/types/base.hpp>
#include <odtone/mih/message.hpp>
#include <odtone/mih/indication.hpp>
#include <odtone/mih/request.hpp>
#include <odtone/mih/response.hpp>
#include <odtone/mih/confirm.hpp>
#include <odtone/mih/tlv_types.hpp>

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <odtone/logger.hpp>
#include <iostream>
#include <memory>

#include "linux/if_80211.hpp"
#include "timer_task.hpp"

#include <wpa_supplicant.hpp>
#include <dhclient.hpp>
#include <boost/filesystem/fstream.hpp>

#define STOP_SCHED_SCAN_ON_L2_UP

namespace po = boost::program_options;
using namespace odtone;

///////////////////////////////////////////////////////////////////////////////
//// Auxiliary Variables and Types
///////////////////////////////////////////////////////////////////////////////

struct periodic_report_data {
	std::unique_ptr<timer_task> task;
	void _report_value(boost::asio::io_service &ios, if_80211 &fi);

	mih::link_param_type type;
};

struct threshold_cross_data {
	bool one_shot;
	mih::link_param_type type;
	mih::threshold th;
};

///////////////////////////////////////////////////////////////////////////////
//// Configuration variables
///////////////////////////////////////////////////////////////////////////////

static const char* const kConf_Sap_Verbosity = "link.verbosity";
static const char* const kConf_Sched_Scan_Period = "link.sched_scan_period";
static const char* const kConf_Default_Threshold_Period = "link.default_th_period";
static const char* const kConf_Resolv_Conf_File = "sys.resolv_conf_file";

static logger log_("sap_80211", std::cout);

///////////////////////////////////////////////////////////////////////////////
//// State variables
///////////////////////////////////////////////////////////////////////////////

std::unique_ptr<sap::link>  ls;
std::unique_ptr<timer_task> threshold_check_task;
std::unique_ptr<timer_task> scheduled_scan_task;

std::unique_ptr<dhcp::dhcpclient> dhc;
std::unique_ptr<wpa_supplicant::Interface> wpa_interface;
std::string resolv_conf_file;
std::string devname;

mih::link_evt_list capabilities_event_list;
mih::link_cmd_list capabilities_command_list;
mih::link_evt_list subscribed_event_list;

boost::shared_mutex _th_list_mutex;
std::vector<threshold_cross_data> th_cross_list;
std::vector<std::unique_ptr<periodic_report_data>> period_rpt_list;

///////////////////////////////////////////////////////////////////////////////
//// Event dispatchers
///////////////////////////////////////////////////////////////////////////////

void clean_l2()
{
	// l2
	try {
		wpa_interface->Disconnect();
	} catch (DBus::Error &e) {
		// was probably already disconnected
	}
}

void clean_l3(if_80211 &fi)
{
	// l3
	fi.clear_addresses();
	fi.clear_routes();
	dhc->release(dhcp::dhcpclient::DHCPv4);
	dhc->release(dhcp::dhcpclient::DHCPv6);

	// dns
	{ boost::filesystem::ofstream dns_resolv(resolv_conf_file, std::ios_base::trunc);
	}
}

void dispatch_link_up(mih::link_tuple_id &lid,
	boost::optional<mih::link_addr> &old_router,
	boost::optional<mih::link_addr> &new_router,
	boost::optional<bool> &ip_renew,
	boost::optional<mih::ip_mob_mgmt> &mobility_management)
{
#ifdef STOP_SCHED_SCAN_ON_L2_UP
	// start the scheduled scan
	if (scheduled_scan_task && scheduled_scan_task->running()) {
		log_(0, "Stopping scheduled scan");
		scheduled_scan_task->stop();
	}
#endif /* STOP_SCHED_SCAN_ON_L2_UP */

	// restart checking the thresholds
	if (th_cross_list.size() > 0 && !threshold_check_task->running()) {
		log_(0, "(cmd) Starting global threshold check task");
		threshold_check_task->start();
	}
	if (period_rpt_list.size() > 0) {
		log_(0, "(cmd) Starting the periodic reports");
		auto it = period_rpt_list.begin();
		while (it != period_rpt_list.end()) {
			it->get()->task->start();
			it++;
		}
	}

	// propagate the event
	if (!subscribed_event_list.get(mih::evt_link_up)) {
		return;
	}

	log_(0, "(event) Dispatching link_up message");

	mih::message m;
	m << mih::indication(mih::indication::link_up)
		& mih::tlv_link_identifier(lid)
		& mih::tlv_old_access_router(old_router)
		& mih::tlv_new_access_router(new_router)
		& mih::tlv_ip_renewal_flag(ip_renew)
		& mih::tlv_ip_mob_mgmt(mobility_management);

	ls->async_send(m);
}

void dispatch_link_down(mih::link_tuple_id &lid,
	boost::optional<mih::link_addr> &old_router,
	mih::link_dn_reason &rs)
{
#ifdef STOP_SCHED_SCAN_ON_L2_UP
	// start scheduled scan
	if (scheduled_scan_task && !scheduled_scan_task->running()) {
		log_(0, "Resuming scheduled scan");
		scheduled_scan_task->start();
	}
#endif /* STOP_SCHED_SCAN_ON_L2_UP */

	// stop parameters report
	if (th_cross_list.size() > 0 && threshold_check_task->running()) {
		log_(0, "(cmd) Stopping global threshold check task");
		threshold_check_task->stop();
	}
	if (period_rpt_list.size() > 0) {
		log_(0, "(cmd) Stopping the periodic reports");
		auto it = period_rpt_list.begin();
		while (it != period_rpt_list.end()) {
			it->get()->task->stop();
			it++;
		}
	}

	// propagate the event
	if (!subscribed_event_list.get(mih::evt_link_down)) {
		return;
	}

	log_(0, "(event) Dispatching link_down message");

	mih::message m;
	m << mih::indication(mih::indication::link_down)
		& mih::tlv_link_identifier(lid)
		& mih::tlv_old_access_router(old_router)
		& mih::tlv_link_dn_reason(rs);

	ls->async_send(m);
}

void dispatch_link_detected(mih::link_det_info &l)
{
	if (!subscribed_event_list.get(mih::evt_link_detected)) {
		return;
	}

	log_(0, "(event) Dispatching link_detected message. SSID \"", l.network_id, "\"");

	mih::message m;
	m << mih::indication(mih::indication::link_detected)
		& mih::tlv_link_det_info(l);

	ls->async_send(m);
}

void dispatch_link_parameters_report(mih::link_tuple_id lid, mih::link_param_rpt_list &rpt_list)
{
	if (!subscribed_event_list.get(mih::evt_link_parameters_report)) {
		return;
	}

	log_(0, "(event) Dispatching link_parameters_report message.");

	mih::message m;
	m << mih::indication(mih::indication::link_parameters_report)
		& mih::tlv_link_identifier(lid)
		& mih::tlv_link_param_rpt_list(rpt_list);

	ls->async_send(m);
}

void dispatch_link_conf_required(mih::link_tuple_id &lid, const std::string &field, const std::string &txt)
{
	if (!subscribed_event_list.get(mih::evt_link_conf_required)) {
		return;
	}

	log_(0, "(event) Dispatching link_conf_required message. \"", field, "\" - \"", txt, "\"");

	mih::configuration_list lconf;
	mih::configuration conf;

	lconf.push_back(conf);

	mih::message m;
	m << mih::indication(mih::indication::link_conf_required)
		& mih::tlv_link_identifier(lid)
		& mih::tlv_configuration_list(lconf);

	ls->async_send(m);
}

void dispatch_link_conf_completion(odtone::uint16 tid, mih::status &status)
{
	mih::message m;
	m.tid(tid);

	if (status != mih::status_success) {
		clean_l2();
	}

	log_(0, "(command) Dispatching link_conf status ", status);
	m << mih::confirm(mih::confirm::link_conf)
		& mih::tlv_status(status);

	ls->async_send(m);
}

bool link_parameter_supported(mih::link_param_type &pt) {
	bool r = false;

	mih::link_param_802_11 *param_802_11 = boost::get<mih::link_param_802_11>(&pt);
	if (param_802_11) {
		if (*param_802_11 == mih::link_param_802_11_rssi) {
			r = true;
		}
	}

	mih::link_param_gen *param_gen = boost::get<mih::link_param_gen>(&pt);
	if (param_gen) {
		if (   *param_gen == mih::link_param_gen_data_rate
		    || *param_gen == mih::link_param_gen_signal_strength
		    || *param_gen == mih::link_param_gen_packet_error_rate) {
			r = true;
		}
	}

	return r;
}

boost::optional<mih::link_param> link_get_parameter(if_80211 &fi, mih::link_param_type &pt) {
	boost::optional<mih::link_param> r;
	mih::link_param status_param;

	poa_info ap_info = fi.get_poa_info();

	mih::link_param_802_11 *param_802_11 = boost::get<mih::link_param_802_11>(&pt);
	if (param_802_11) {
		status_param.type = *param_802_11;

		if (*param_802_11 == mih::link_param_802_11_rssi) {
			if (mih::percentage *v = boost::get<mih::percentage>(&ap_info.signal)) {
				status_param.value = (odtone::uint)*v;
			} else if (odtone::sint8 *v = boost::get<odtone::sint8>(&ap_info.signal)) {
				status_param.value = *v;
			}
			r.reset(status_param);
		} else if (*param_802_11 == mih::link_param_802_11_no_qos) {
			status_param.value = !ap_info.net_capabilities.get(mih::net_caps_qos_0);
			r.reset(status_param);
		//} else if (*param_802_11 == mih::link_param_802_11_multicast_packet_loss_rate) {
		}
	}

	mih::link_param_gen *param_gen = boost::get<mih::link_param_gen>(&pt);
	if (param_gen) {
		status_param.type = *param_gen;

		if (*param_gen == mih::link_param_gen_data_rate) {
			mih::mac_addr addr = boost::get<mih::mac_addr>(boost::get<mih::link_addr>(ap_info.id.poa_addr));
			status_param.value = fi.get_current_data_rate(addr);
			r.reset(status_param);
		} else if (*param_gen == mih::link_param_gen_signal_strength) {
			if (mih::percentage *v = boost::get<mih::percentage>(&ap_info.signal)) {
				status_param.value = (odtone::uint)*v;
			} else if (odtone::sint8 *v = boost::get<odtone::sint8>(&ap_info.signal)) {
				status_param.value = *v;
			}
			r.reset(status_param);
		} else if (*param_gen == mih::link_param_gen_packet_error_rate) {
			status_param.value = fi.get_packet_error_rate();
			r.reset(status_param);
		//} else if (*param_gen == mih::link_param_gen_sinr) {
		//} else if (*param_gen == mih::link_param_gen_throughput) {
		}
	}

	return r;
}

// For cross-value-alert threshold types
void global_thresholds_check(boost::asio::io_service &ios, if_80211 &fi)
{
	if (!subscribed_event_list.get(mih::evt_link_parameters_report)) {
		return;
	}

	log_(0, "(cmd) Performing periodic threshold check");

	try {
		mih::link_tuple_id lid = fi.link_tuple_id();
		mih::link_param_rpt_list rpt_list;

		boost::unique_lock<boost::shared_mutex> lock(_th_list_mutex);

		std::vector<threshold_cross_data>::iterator th_it = th_cross_list.begin();
		while (th_it != th_cross_list.end()) {
			boost::optional<boost::variant<mih::link_param_val, mih::qos_param_val>> value;

			boost::optional<mih::link_param> status_param = link_get_parameter(fi, th_it->type);

			if (!status_param) {
				continue;
			}

			uint16 uintval = boost::get<mih::link_param_val>(status_param.get().value);

			if (th_it->th.threshold_x_dir == mih::threshold::above_threshold) {
				if (static_cast<int>(uintval) > static_cast<int>(th_it->th.threshold_val)) {
					value = status_param.get().value;
				}
			} else if (th_it->th.threshold_x_dir == mih::threshold::below_threshold) {
				if (static_cast<int>(uintval) < static_cast<int>(th_it->th.threshold_val)) {
					value = status_param.get().value;
				}
			}

			if (value) {
				mih::link_param_report rpt;
				rpt.param.type = th_it->type;
				rpt.param.value = value.get();
				rpt.thold = th_it->th;
				rpt_list.push_back(rpt);
			}

			if (value && th_it->one_shot) {
				log_(0, "(link) Removing one-shot type threshold from list");
				th_it = th_cross_list.erase(th_it);
			} else {
				++th_it;
			}
		}

		if (rpt_list.size() > 0) {
			ios.dispatch(boost::bind(&dispatch_link_parameters_report, lid, rpt_list));
		}
	} catch (const std::exception &e) {
		log_(0, "(cmd) Unable to get associated POA info: ", e.what());
		return;
	}
}

// For periodic metric reporting
void periodic_report_data::_report_value(boost::asio::io_service &ios, if_80211 &fi)
{
	// Periodic reports don't need to be subscribed?
//	if (!subscribed_event_list.get(mih::evt_link_parameters_report)) {
//		return;
//	}

	log_(0, "(cmd) Handling periodic report");

	try {
		boost::shared_lock<boost::shared_mutex> lock(_th_list_mutex);

		boost::optional<mih::link_param> status_param = link_get_parameter(fi, type);
		if (status_param) {
			mih::link_param_report rpt;
			rpt.param = status_param.get();

			mih::link_param_rpt_list rpt_list;
			rpt_list.push_back(rpt);

			ios.dispatch(boost::bind(&dispatch_link_parameters_report, fi.link_tuple_id(), rpt_list));
		}
	} catch (const std::exception &e) {
		log_(0, "(cmd) Error handling periodic report: ", e.what());
	}
}

void scheduled_scan_trigger(if_80211 &fi)
{
	log_(0, "Triggering scheduled scan");
	try {
		fi.trigger_scan(false);
	} catch(const std::exception &e) {
		log_(0, "Error triggering scheduled scan: ", e.what());
	}
}

///////////////////////////////////////////////////////////////////////////////
//// Command handling functions
///////////////////////////////////////////////////////////////////////////////

template <class T>
DBus::Variant to_variant(const T &value)
{
	DBus::Variant v;
	DBus::MessageIter iter = v.writer();
	iter << value;
	return v;
}

// Dispatch failure message for command errors.
void dispatch_status_failure(odtone::uint16 tid, mih::confirm::mid mid)
{
	log_(0, "(command) Dispatching status_failure");

	mih::message m;
	mih::status st = mih::status_failure;

	m << mih::confirm(mid)
		& mih::tlv_status(st);

	m.tid(tid);

	ls->async_send(m);
}

// Dispatch a capability_discover confirm.
// Fully supported, but check the supported events and commands lists.
void handle_capability_discover(odtone::uint16 tid)
{
	log_(0, "(command) Handling capability_discover");

	log_(0, "(command) Dispatching status success");

	mih::message m;
	mih::status st = mih::status_success;

	m << mih::confirm(mih::confirm::capability_discover)
		& mih::tlv_status(st)
		& mih::tlv_link_evt_list(capabilities_event_list)
		& mih::tlv_link_cmd_list(capabilities_command_list);

	m.tid(tid);

	ls->async_send(m);
}

// Dispatch an event_subscribe confirm.
// Fully supported, but check the supported events list.
void handle_event_subscribe(odtone::uint16 tid, mih::link_evt_list &events)
{
	log_(0, "(command) Handling event_subscribe");

	// Set the common events between the requested and the supported
	// (will not successfully subscribe unsupported events...)
	events.common(capabilities_event_list);

	// Save the new list (don't unsubscribe previous bits!)
	subscribed_event_list.merge(events);

	log_(0, "(command) Dispatching status success");

	mih::message m;
	mih::status st = mih::status_success;

	m << mih::confirm(mih::confirm::event_subscribe)
		& mih::tlv_status(st)
		& mih::tlv_link_evt_list(events);

	m.tid(tid);

	ls->async_send(m);
}

// Dispatch an event unsubscribe confirm.
// Fully supported, but check the supported events list.
void handle_event_unsubscribe(odtone::uint16 tid, mih::link_evt_list &events)
{
	log_(0, "(command) Handling event_unsubscribe");

	// Set the common events between the requested and the supported
	// (will not successfully unsubscribe unsupported events...)
	events.common(capabilities_event_list);

	// Remove the requested events from the subscription list
	subscribed_event_list.except(events);

	log_(0, "(command) Dispatching status success");

	mih::message m;
	mih::status st = mih::status_success;

	m << mih::confirm(mih::confirm::event_unsubscribe)
		& mih::tlv_status(st)
		& mih::tlv_link_evt_list(events);

	m.tid(tid);

	ls->async_send(m);
}

// Dispatch a link_get_parameters confirm.
// Partially supported.
//
// Fails if scan fails to return an associated network.
// Aditionally, will fail if requesting any parameter outside of this list:
// - link_param_802_11_rssi, link_states_req_op_mode, link_states_req_channel_id
void handle_link_get_parameters(if_80211 &fi,
	odtone::uint16 tid,
	mih::link_param_type_list &param_list,
	mih::link_states_req &states_req,
	mih::link_desc_req &desc_req)
{
	log_(0, "(command) Handling link_get_parameters");

	try {
		mih::link_param_list status_list;

		BOOST_FOREACH (mih::link_param_type &pt, param_list) {
			boost::optional<mih::link_param> status_param = link_get_parameter(fi, pt);
			if (status_param) {
				status_list.push_back(status_param.get());
			} else {
				log_(0, "(command) No support for specified link_param");
			}
		}

		mih::link_states_rsp_list states_list;
		mih::link_states_rsp states_param;

		poa_info ap_info = fi.get_poa_info();

		if (states_req.get(mih::link_states_req_channel_id)) {
			states_param = ap_info.channel_id;
			states_list.push_back(states_param);
		}
		if (states_req.get(mih::link_states_req_op_mode)) {
			states_param = fi.get_op_mode();
			states_list.push_back(states_param);
		}

		mih::link_desc_rsp_list desc_list;
		if (desc_req.get(mih::link_desc_req_classes_of_service_supported)) {
			log_(0, "(command) No support for specified link_desc_req");
			//dispatch_status_failure(tid, mih::confirm::link_get_parameters);
			//return;
		}
		if (desc_req.get(mih::link_desc_req_queues_supported)) {
			log_(0, "(command) No support for specified link_desc_req");
			//dispatch_status_failure(tid, mih::confirm::link_get_parameters);
			//return;
		}

		log_(0, "(command) Dispatching status success");

		mih::message m;
		mih::status st = mih::status_success;

		m << mih::confirm(mih::confirm::link_get_parameters)
			& mih::tlv_status(st)
			& mih::tlv_link_parameters_status_list(status_list)
			& mih::tlv_link_states_rsp(states_list)
			& mih::tlv_link_descriptor_rsp(desc_list);

		m.tid(tid);

		ls->async_send(m);
	} catch (const std::exception &e) {
		log_(0, "(command) Exception: ", e.what());
		dispatch_status_failure(tid, mih::confirm::link_get_parameters);
	}
}

// Dispatch a link_configure_thresholds confirm
void handle_link_configure_thresholds(boost::asio::io_service &ios,
	if_80211 &fi,
	odtone::uint16 tid,
	mih::link_cfg_param_list &param_list)
{
	log_(0, "(command) Handling link_configure_thresholds");

	mih::link_cfg_status_list status_list;

	std::vector<threshold_cross_data>::iterator cross_it;
	std::vector<std::unique_ptr<periodic_report_data>>::iterator rpt_it;

	BOOST_FOREACH (mih::link_cfg_param &param, param_list) {
		// check for support
		if (!link_parameter_supported(param.type)) {
			log_(0, "(command) No support for specified link_param_type");
			mih::link_cfg_status status;
			status.type = param.type;
			status.status = false;
			status_list.push_back(status);
			continue;
		}

		boost::unique_lock<boost::shared_mutex> lock(_th_list_mutex);
		if (param.action == mih::th_action_cancel) { // cancel thresholds
			if (param.threshold_list.size() == 0) { // cancel all of type 'type'
				log_(0, "(command) Cancelling all configured thresholds of type ", param.type);

				cross_it = th_cross_list.begin();
				while (cross_it != th_cross_list.end()) {
					if (cross_it->type == param.type) {
						cross_it = th_cross_list.erase(cross_it);
					} else {
						++cross_it;
					}
				}

				rpt_it = period_rpt_list.begin();
				while (rpt_it != period_rpt_list.end()) {
					if (rpt_it->get()->type == param.type) {
						rpt_it = period_rpt_list.erase(rpt_it);
					} else {
						++rpt_it;
					}
				}

				mih::link_cfg_status status;
				status.type = param.type;
				status.status = true;
				status_list.push_back(status);
			} else { // cancel specific only.
				log_(0, "(command) Cancelling some thresholds of type ", param.type);

				BOOST_FOREACH (mih::threshold &th, param.threshold_list) {
					cross_it = th_cross_list.begin();
					while (cross_it != th_cross_list.end()) {
						if (cross_it->type == param.type
							&& cross_it->th.threshold_val == th.threshold_val
							&& cross_it->th.threshold_x_dir == th.threshold_x_dir) {
							cross_it = th_cross_list.erase(cross_it);
						} else {
							++cross_it;
						}
					}

					mih::link_cfg_status status;
					status.type = param.type;
					status.thold = th;
					status.status = true;
					status_list.push_back(status);
				}
			}
		} else { // insert it
			odtone::uint16 *period = boost::get<odtone::uint16>(&param.timer_interval);
			if (period) { // insert periodic report
				log_(0, "(command) Inserting periodic report");

				std::unique_ptr<periodic_report_data> p(new periodic_report_data);
				p->type = param.type;
				p->task.reset(new timer_task(ios, *period,
					boost::bind(&periodic_report_data::_report_value, p.get(), boost::ref(ios), boost::ref(fi))));

				p->task->start();
				period_rpt_list.push_back(std::move(p));
			} else { // insert "value-cross" alerts
				BOOST_FOREACH (mih::threshold &th, param.threshold_list) {
					log_(0, "(command) Inserting value-cross-alert threshold");

					threshold_cross_data t;
					t.one_shot = (param.action == mih::th_action_one_shot);
					t.type = param.type;
					t.th = th;
					th_cross_list.push_back(t);

					mih::link_cfg_status status;
					status.type = param.type;
					status.thold = th;
					status.status = true;
					status_list.push_back(status);
				}
			}
		}
	}

	// Start/stop periodic threshold check based on threshold configuration existence
	if (th_cross_list.size() > 0 && !threshold_check_task->running()) {
		log_(0, "(cmd) New thresholds configured, starting global check task");
		threshold_check_task->start();
	} else if (th_cross_list.size() == 0 && threshold_check_task->running()) {
		log_(0, "(cmd) All thresholds removed, stopping global check task");
		threshold_check_task->stop();
	}

	log_(0, "(cmd) Dispatching status success");

	mih::message m;
	mih::status st = mih::status_success;

	m << mih::confirm(mih::confirm::link_configure_thresholds)
		& mih::tlv_status(st)
		& mih::tlv_link_cfg_status_list(status_list);

	m.tid(tid);

	ls->async_send(m);
}

// Dispatch a link actions confirm.
// Partially supported.
//
// Will fail if requested attributes include:
// - data_fwd_req, link_res_retain
//
// Scan might fail if action type is power_down.
//
// Aditionally, a failure will occur if the process has no
// permission to perform such actions on the link.
//
// Low power mode cannot be enforced here,
// instead we just enable power saving features on the device.
void handle_link_actions(boost::asio::io_service &ios,
	if_80211 &fi,
	odtone::uint16 tid,
	mih::link_action &action,
	odtone::uint16 &delay,
	boost::optional<mih::link_addr> &poa)
{
	log_(0, "(command) Handling link_action");

	try {
		// This should be processed after the delay, but
		// it's being processed before due to the lack of support
		if (action.attr.get(mih::link_ac_attr_res_retain)) {
			// not supported
			log_(0, "(command) No support for specified link_ac_attr");
			dispatch_status_failure(tid, mih::confirm::link_actions);
			return;
		}
		if (action.attr.get(mih::link_ac_attr_data_fwd_req)) {
			// not supported
			log_(0, "(command) No support for specified link_ac_attr");
			dispatch_status_failure(tid, mih::confirm::link_actions);
			return;
		}
		if (poa) {
			// not supported
			log_(0, "(command) No support for specified attribute (poa addr)");
			dispatch_status_failure(tid, mih::confirm::link_actions);
			return;
		}

		if (delay > 0) {
			boost::asio::deadline_timer timer(ios);
			timer.expires_from_now(boost::posix_time::milliseconds(delay));
			timer.wait();
			// Should we async_wait, and free the link_sap for more message processing?
		}

		fi.set_op_mode(action.type.get());

		// get scan results
		mih::link_scan_rsp_list scan_rsp_list;
		if (action.type != mih::link_ac_type_power_down) {
			if (action.attr.get(mih::link_ac_attr_scan)) {
				fi.trigger_scan(true);
				scan_rsp_list = fi.get_scan_results();
			}
		}

		if (   action.type == mih::link_ac_type_power_down
		    || action.type == mih::link_ac_type_disconnect) {
			clean_l2();
			clean_l3(fi);
		}

		log_(0, "(command) Dispatching status success");

		mih::message m;
		mih::status st = mih::status_success;

		m << mih::confirm(mih::confirm::link_actions)
			& mih::tlv_status(st)
			& mih::tlv_link_scan_rsp_list(scan_rsp_list)
			& mih::tlv_link_ac_result(mih::link_ac_success);

		m.tid(tid);

		ls->async_send(m);
	} catch (const std::exception &e) {
		// This status_failure is tricky. For example, the scan operation
		// may throw an exception, but the oper_mode is still correctly set!
		log_(0, "(command) Exception: ", e.what());
		dispatch_status_failure(tid, mih::confirm::link_actions);
	}
}

// This basically requests a connection to the poa identified by the link_tuple_id
// and network, given a requested set of configurations
void handle_link_conf(const boost::asio::io_service &ios,
                      if_80211 &fi,
                      odtone::uint16 tid,
                      const boost::optional<mih::link_addr> &poa,
                      const mih::configuration_list &lconf)
{
	log_(0, "(command) Handling link_conf");

	try {
		std::map<std::string, DBus::Variant> network_map;
		auto it = lconf.begin();
		while (it != lconf.end()) {
			log_(0, "(command) supplicant setting [", it->key, "=",
				   boost::iequals(it->key, "password")
				|| boost::iequals(it->key, "psk") ? "\"secret\"" : it->value, "]");

			if (   boost::iequals(it->key, "scan_ssid")
			    || boost::iequals(it->key, "mode")
			    || boost::iequals(it->key, "frequency")
			    || boost::iequals(it->key, "wep_tx_keyidx")
			    || boost::iequals(it->key, "eapol_flags")
			    || boost::iequals(it->key, "engine")
			    || boost::iequals(it->key, "fragment_size")
			    || boost::iequals(it->key, "proactive_key_caching")) {
				network_map[it->key] = to_variant(boost::lexical_cast<int>(it->value));
			} else {
				network_map[it->key] = to_variant(it->value);
			}
			it ++;
		}

		log_(0, "(command) Attempting connection");

		try {
			DBus::Path wpa_network = wpa_interface->AddNetwork(network_map);
			wpa_interface->add_completion_handler(boost::bind(dispatch_link_conf_completion, tid, _1));
			wpa_interface->SelectNetwork(wpa_network);
		} catch (DBus::Error &e) {
			log_(0, "(command) DBus error \"", e.name(), ": ", e.message(), "\"");
			dispatch_status_failure(tid, mih::confirm::link_conf);
			return;
		}
	} catch (std::exception &e) {
		log_(0, "(command) Exception: ", e.what());
		dispatch_status_failure(tid, mih::confirm::link_conf);
	}
}

void handle_l3_conf(const boost::asio::io_service &ios,
                    if_80211 &fi,
                    odtone::uint16 tid,
                    const mih::ip_cfg_methods &cfg_methods,
                    const boost::optional<mih::ip_info_list> &address_list,
                    const boost::optional<mih::ip_info_list> &route_list,
                    const boost::optional<mih::ip_addr_list> &dns_list,
                    const boost::optional<mih::fqdn_list> &domain_list)
{
	log_(0, "(command) Handling l3_conf");

	try {
		log_(0, "(command) Clearing previous configurations");

		// cleanup previous configurations
		clean_l3(fi);

		// handle automatic configurations
		if (cfg_methods.get(mih::ip_cfg_ipv4_dynamic)) {
			log_(0, "(command) Adding dynamic IPv4 configuration");
			dhc->bind(dhcp::dhcpclient::DHCPv4);
		}
		if (cfg_methods.get(mih::ip_cfg_ipv6_stateful)) {
			log_(0, "(command) Adding dynamic IPv6 configuration");
			dhc->bind(dhcp::dhcpclient::DHCPv6);
		}

		// handle static configurations
		if (address_list) {
			log_(0, "(command) Adding static addresses");
			fi.add_addresses(address_list.get());
		}
		if (route_list) {
			log_(0, "(command) Adding static routes");
			fi.add_routes(route_list.get());
		}
		if (dns_list) {
			log_(0, "(command) Adding manual dns servers");
			boost::filesystem::ofstream dns_resolv(resolv_conf_file, std::ios_base::app);
			for (auto it = dns_list.get().begin(); it != dns_list.get().end(); ++it) {
				dns_resolv << "nameserver " << it->address() << std::endl;
			}
		}
		if (domain_list) {
			log_(0, "(command) Adding manual domain list");
			std::string searches = boost::algorithm::join(domain_list.get(), " ");
			boost::filesystem::ofstream dns_resolv(resolv_conf_file, std::ios_base::app);
			dns_resolv << "search " << searches << std::endl;
		}

		log_(0, "(command) Dispatching status success");

		mih::message m;
		mih::status st = mih::status_success;

		m << mih::confirm(mih::confirm::l3_conf)
			& mih::tlv_status(st);

		m.tid(tid);

		ls->async_send(m);
	} catch (std::exception &e) {
		log_(0, "(command) Exception: ", e.what());
		dispatch_status_failure(tid, mih::confirm::l3_conf);
	}
}

///////////////////////////////////////////////////////////////////////////////
//// Special MIHF callback
///////////////////////////////////////////////////////////////////////////////

void default_handler(boost::asio::io_service &ios,
	if_80211 &fi,
	mih::message& msg,
	const boost::system::error_code& ec)
{
	if (ec) {
		return;
	}

	switch (msg.mid()) {
	case mih::request::capability_discover:
		{
			log_(0, "(command) Received capability_discover message");
			ios.dispatch(boost::bind(&handle_capability_discover, msg.tid()));
		}
		break;

	case mih::request::event_subscribe:
		{
			log_(0, "(command) Received event_subscribe message");
			mih::link_evt_list events;
				msg >> mih::request()
				& mih::tlv_link_evt_list(events);

			ios.dispatch(boost::bind(&handle_event_subscribe, msg.tid(), events));
		}
		break;

	case mih::request::event_unsubscribe:
		{
			log_(0, "(command) Received event_unsubscribe message");
			mih::link_evt_list events;
			msg >> mih::request()
				& mih::tlv_link_evt_list(events);

			ios.dispatch(boost::bind(&handle_event_unsubscribe, msg.tid(), events));
		}
		break;

	case mih::request::link_get_parameters:
		{
			log_(0, "(command) Received link_get_parameters message");
			mih::message m;

			mih::link_param_type_list param_list;
			mih::link_states_req states_req;
			mih::link_desc_req desc_req;

			msg >> mih::request()
				& mih::tlv_link_parameters_req(param_list)
				& mih::tlv_link_states_req(states_req)
				& mih::tlv_link_descriptor_req(desc_req);

			ios.dispatch(boost::bind(&handle_link_get_parameters, boost::ref(fi), msg.tid(),
			                                                      param_list, states_req, desc_req));
		}
		break;

	case mih::request::link_configure_thresholds:
		{
			log_(0, "(command) Received link_configure_thresholds message");
			mih::link_cfg_param_list param_list;

			msg >> mih::request()
				& mih::tlv_link_cfg_param_list(param_list);

			ios.dispatch(boost::bind(&handle_link_configure_thresholds, boost::ref(ios), boost::ref(fi), msg.tid(),
			                                                            param_list));
		}
		break;

	case mih::request::link_actions:
		{
			log_(0, "(command) Received link_actions message");
			mih::link_action action;
			odtone::uint16 delay;
			boost::optional<mih::link_addr> poa;

			msg >> mih::request()
				& mih::tlv_link_action(action)
				& mih::tlv_time_interval(delay)
				& mih::tlv_poa(poa);

			ios.dispatch(boost::bind(&handle_link_actions, boost::ref(ios), boost::ref(fi), msg.tid(),
			                                               action, delay, poa));
		}
		break;

	case mih::request::link_conf:
		{
			log_(0, "(command) Received link_conf message");

			mih::link_tuple_id lti;
			boost::optional<mih::link_addr> poa;
			mih::configuration_list lconf;

			msg >> mih::request()
				& mih::tlv_link_identifier(lti)
				& mih::tlv_poa(poa)
				& mih::tlv_configuration_list(lconf);

			ios.dispatch(boost::bind(&handle_link_conf, boost::ref(ios), boost::ref(fi), msg.tid(),
			                                            poa, lconf));
		}
		break;

	case mih::request::l3_conf:
		{
			log_(0, "(command) Received l3_conf message");

			mih::link_tuple_id lti;
			mih::ip_cfg_methods cfg_methods;
			boost::optional<mih::ip_info_list> address_list;
			boost::optional<mih::ip_info_list> route_list;
			boost::optional<mih::ip_addr_list> dns_list;
			boost::optional<mih::fqdn_list> domain_list;

			msg >> mih::request()
				& mih::tlv_link_identifier(lti)
				& mih::tlv_ip_cfg_methods(cfg_methods)
				& mih::tlv_ip_addr_list(address_list)
				& mih::tlv_ip_route_list(route_list)
				& mih::tlv_ip_dns_list(dns_list)
				& mih::tlv_fqdn_list(domain_list);

			ios.dispatch(boost::bind(&handle_l3_conf, boost::ref(ios), boost::ref(fi), msg.tid(),
			                                          cfg_methods, address_list, route_list, dns_list, domain_list));
		}
		break;

	default:
		log_(0, "(command) Received unknown MIH message");
	}
}

///////////////////////////////////////////////////////////////////////////////
//// Auxiliary Functions
///////////////////////////////////////////////////////////////////////////////

// To send link_register message to the MIHF.
void mihf_sap_init(mih::link_id &id)
{
	mih::message m;

	m << mih::indication(mih::indication::link_register)
		& mih::tlv_interface_type_addr(id);

	ls->async_send(m);
}

// To set the supported event list. (hardcoded)
void set_supported_event_list()
{
	capabilities_event_list.set(mih::evt_link_detected);
	capabilities_event_list.set(mih::evt_link_up);
	capabilities_event_list.set(mih::evt_link_down);
	capabilities_event_list.set(mih::evt_link_parameters_report);
	//capabilities_event_list.set(mih::evt_link_going_down);
	//capabilities_event_list.set(mih::evt_link_handover_imminent);
	//capabilities_event_list.set(mih::evt_link_handover_complete);
	//capabilities_event_list.set(mih::evt_link_pdu_transmit_status);
	capabilities_event_list.set(mih::evt_link_conf_required);
}

// To set the supported command list. (hardcoded)
void set_supported_command_list()
{
	capabilities_command_list.set(mih::cmd_link_event_subscribe);
	capabilities_command_list.set(mih::cmd_link_event_unsubscribe);
	capabilities_command_list.set(mih::cmd_link_get_parameters);
	capabilities_command_list.set(mih::cmd_link_configure_thresholds);
	capabilities_command_list.set(mih::cmd_link_conf);
	capabilities_command_list.set(mih::cmd_l3_conf);
}

///////////////////////////////////////////////////////////////////////////////
//// Main
///////////////////////////////////////////////////////////////////////////////

void dbus_dispatch_pending(DBus::BusDispatcher &d, boost::mutex &m)
{
	d.dispatch_pending();
	m.unlock();
}

int main(int argc, char** argv)
{
	setup_crash_handler();

	if (geteuid()) {
		std::cerr << "###########" << std::endl;
		std::cerr << "# WARNING: some functionalities may fail without root privileges" << std::endl;
		std::cerr << "###########" << std::endl;
	}

	// Declare Link SAP available options
	po::options_description desc("MIH Link SAP Configuration");
	desc.add_options()
		("help", "Display configuration options")
		(kConf_Sap_Verbosity, po::value<odtone::uint>()->default_value(2), "Log level [0-2]")
		(kConf_Sched_Scan_Period, po::value<odtone::uint>()->default_value(0), "Scheduled scan interval (millis)")
		(kConf_Default_Threshold_Period, po::value<odtone::uint>()->default_value(1000), "Default threshold checking interval (millis)")
		(sap::kConf_Interface_Addr, po::value<std::string>()->default_value(""), "Interface address")
		(sap::kConf_Port, po::value<odtone::ushort>()->default_value(1235), "Port")
		(sap::kConf_File, po::value<std::string>()->default_value("sap_80211.conf"), "Configuration File")
		(sap::kConf_Receive_Buffer_Len, po::value<odtone::uint>()->default_value(4096), "Receive Buffer Length")
		(sap::kConf_MIHF_Ip, po::value<std::string>()->default_value("127.0.0.1"), "Local MIHF Ip")
		(sap::kConf_MIHF_Local_Port, po::value<odtone::ushort>()->default_value(1025), "MIHF Local Communications Port")
		(sap::kConf_MIHF_Id, po::value<std::string>()->default_value("local-mihf"), "Local MIHF Id")
		(sap::kConf_MIH_SAP_id, po::value<std::string>()->default_value("link"), "Link SAP Id")
		(kConf_Resolv_Conf_File, po::value<std::string>()->default_value("/etc/resolv.conf"), "System's resolv.conf location");

	mih::config cfg(desc);
	cfg.parse(argc, argv, sap::kConf_File);

	if (cfg.help()) {
		std::cerr << desc << std::endl;
		return EXIT_SUCCESS;
	}

	resolv_conf_file = cfg.get<std::string>(kConf_Resolv_Conf_File);

	odtone::uint sched_scan_period = cfg.get<odtone::uint>(kConf_Sched_Scan_Period);
	odtone::uint th_period = cfg.get<odtone::uint>(kConf_Default_Threshold_Period);
	if (th_period == 0) {
		std::cerr << "default_th_period must be positive!" << std::endl;
		return EXIT_FAILURE;
	}

	set_supported_event_list();
	set_supported_command_list();

	// start dbus service
	DBus::BusDispatcher dispatcher;
	DBus::default_dispatcher = &dispatcher;
	DBus::Connection dbus_connection = DBus::Connection::SystemBus();

	boost::asio::io_service ios;

	if_80211 fi(ios, mih::mac_addr(cfg.get<std::string>(sap::kConf_Interface_Addr)));
	mih::link_tuple_id id = fi.link_tuple_id();

	ls.reset(new sap::link(cfg, ios, boost::bind(&default_handler, boost::ref(ios), boost::ref(fi), _1, _2)));
	mihf_sap_init(id);

	threshold_check_task.reset(new timer_task(ios, th_period,
		boost::bind(&global_thresholds_check, boost::ref(ios), boost::ref(fi))));

	if (sched_scan_period > 0) {
		scheduled_scan_task.reset(new timer_task(ios, sched_scan_period,
			boost::bind(&scheduled_scan_trigger, boost::ref(fi))));
#ifndef STOP_SCHED_SCAN_ON_L2_UP
		scheduled_scan_task->start();
#else
		if (!fi.link_up()) {
			scheduled_scan_task->start();
		}
#endif /* STOP_SCHED_SCAN_ON_L2_UP */
	}

	fi.link_up_callback(boost::bind(&dispatch_link_up, _1, _2, _3, _4, _5));
	fi.link_down_callback(boost::bind(&dispatch_link_down, _1, _2, _3));
	fi.link_detected_callback(boost::bind(&dispatch_link_detected, _1));

	devname = fi.ifname();

	// checking wpa_supplicant and dhcpcd
	sleep(1); // TODO: "ensure" the d-bus dispatcher has started


	dhc.reset(new dhcp::dhclient(devname));

	wpa_supplicant::WPASupplicant wpa(dbus_connection, "/fi/w1/wpa_supplicant1", "fi.w1.wpa_supplicant1");

	DBus::Path wpa_interface_path;
	try {
		wpa_interface_path = wpa.GetInterface(devname);
	} catch (DBus::Error &e) {
		log_(0, "Error getting ", devname, " interface from WPA Supplicant. Adding it myself.");
		log_(0, "Message was \"", e.name(), ": ", e.message(), "\"");

		std::map<std::string, DBus::Variant> m;

		m["Ifname"] = to_variant(devname);
		//m["Driver"] = to_variant(std::string("nl80211"));

		wpa_interface_path = wpa.CreateInterface(m);

		log_(0, "Apparently successul, at ", wpa_interface_path);
	}

	wpa_interface.reset(new wpa_supplicant::Interface(dbus_connection,
	                                                  wpa_interface_path.c_str(),
	                                                  "fi.w1.wpa_supplicant1",
	                                                  boost::bind(&dispatch_link_conf_required, boost::ref(id), _1, _2)));

	boost::thread io(boost::bind(&boost::asio::io_service::run, &ios));

	boost::mutex m;
	while (true) {
		ios.dispatch(boost::bind(&dbus_dispatch_pending, boost::ref(dispatcher), boost::ref(m)));
		m.lock();
		dispatcher.dispatch();
	}

	fi.link_up_callback(boost::bind(&dispatch_link_up, _1, _2, _3, _4, _5));
	fi.link_down_callback(boost::bind(&dispatch_link_down, _1, _2, _3));
	fi.link_detected_callback(boost::bind(&dispatch_link_detected, _1));

	ios.run();
}

// EOF ////////////////////////////////////////////////////////////////////////
