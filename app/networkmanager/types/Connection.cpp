//==============================================================================
// Brief   : NetworkManager Connection interface implementation
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

#include "Connection.hpp"

#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

static const std::string k_file_discriminator = "file://";

using namespace odtone::networkmanager;

Connection::Connection(DBus::Connection &connection,
                       const char* path,
                       std::map<DBus::Path, std::shared_ptr<Connection>> &container,
                       const boost::filesystem::path &file_path) :
	DBus::ObjectAdaptor(connection, path),
	_file_path(file_path),
	_container(container),
	_path(path),
	log_(_path.c_str(), std::cout)
{
	read_settings();
}

Connection::Connection(DBus::Connection &connection,
                       const char* path,
                       std::map<DBus::Path, std::shared_ptr<Connection>> &container,
                       const settings_map &settings,
                       const boost::filesystem::path &file_path) :
	DBus::ObjectAdaptor(connection, path),
	_file_path(file_path),
	_settings(settings),
	_container(container),
	_path(path),
	log_(_path.c_str(), std::cout)
{
	// TODO calculate uuid?
	write_settings();
}

Connection::~Connection()
{
}

settings_map Connection::GetSecrets(const std::string& setting_name)
{
	log_(0, "Getting connection secrets");

	settings_map r;
	// not supported

	log_(0, "Done");

	return r;
}

settings_map Connection::GetSettings()
{
	log_(0, "Getting connection settings");

	log_(0, "Done");

	return _settings;
}

void Connection::Delete()
{
	log_(0, "Deleting myself...");

	// signal removal
	Removed();

	log_(0, "Removing file");
	boost::filesystem::remove(_file_path);

	log_(0, "Removing from container");
	_container.erase(_container.find(_path));
}

void Connection::Update(const settings_map &properties)
{
	log_(0, "Updating settings locally");

	// thecnically the map will be different once reloaded again from file!
	_settings = properties;
	write_settings();

	// signal changes
	Updated();

	log_(0, "Done");
}

std::string Connection::GetUuid()
{
	// TODO accessing the element in this way creates a default value, use find instead!
	return _settings["connection"]["uuid"];
}

// auxiliary functions for WEP key operations
unsigned char hex2byte(const std::string &hex)
{
	unsigned short byte = 0;
	std::istringstream iss(hex);
	iss >> std::hex >> byte;
	return byte % 0x100;
}

std::string hex2bin(const std::string &hex)
{
	std::string bin;

	for (unsigned int r = 1; r < hex.size(); r += 2) {
		bin += hex2byte(hex.substr(r - 1, 2));
	}

	return bin;
}

std::string get_key(std::string &key, int key_type)
{
	std::string result;

	if (key_type == 0) { // KEY
		if (key.size() == 10 || key.size() == 26) { //
			result = hex2bin(key);
		} else if (key.size() == 5 || key.size() == 13) {
			result = key;
		}
	} else { // PASSPHRASE
		// TODO
		// result = md5(key);
	}

	return result;
}

std::map<std::string, std::string> Connection::supplicant_conf()
{
	log_(0, "Translating settings to wpa_supplicant format");

	std::map<std::string, std::string> supp;
	connection_settings s(_settings);

	// categories from NetworkManager src/supplicant-manager/nm-supplicant-config.c
	//
	// setting_wireless
	//

	// adhoc
	// not supported, so set scan_ssid to 1
	supp["scan_ssid"] = "1";

	// ssid
	if (s.set_wireless._ssid) {
		supp["ssid"] = std::string(s.set_wireless._ssid.get().begin(), s.set_wireless._ssid.get().end());
	}

	// bssid
	if (s.set_wireless._bssid) {
		supp["bssid"] = std::string(s.set_wireless._bssid.get().begin(), s.set_wireless._bssid.get().end());
	}

	//
	// setting_wireless_security
	//

	// key-mgmt
	if (s.set_wireless_security._key_mgmt) {
		std::string case_sensitive = s.set_wireless_security._key_mgmt.get();
		boost::to_upper(case_sensitive);
		supp["key_mgmt"] = case_sensitive;

		// wpa
		if (boost::iequals(s.set_wireless_security._key_mgmt.get(), "wpa-none") ||
			boost::iequals(s.set_wireless_security._key_mgmt.get(), "wpa-psk") ||
			boost::iequals(s.set_wireless_security._key_mgmt.get(), "wpa-eap")) {
			// proto
			if (s.set_wireless_security._proto) {
				case_sensitive = boost::algorithm::join(s.set_wireless_security._proto.get(), " ");
				boost::to_upper(case_sensitive);
				supp["proto"] = case_sensitive;
			}


			// pairwise
			if (s.set_wireless_security._pairwise) {
				case_sensitive = boost::algorithm::join(s.set_wireless_security._pairwise.get(), " ");
				boost::to_upper(case_sensitive);
				supp["pairwise"] = case_sensitive;
			}


			// group
			if (s.set_wireless_security._group) {
				case_sensitive = boost::algorithm::join(s.set_wireless_security._group.get(), " ");
				boost::to_upper(case_sensitive);
				supp["group"] = case_sensitive;
			}

		}

		// wep
		// else //?
		// default to type key
		int key_type = s.set_wireless_security._wep_key_type ? s.set_wireless_security._wep_key_type.get() : 1;
		if (boost::iequals(s.set_wireless_security._key_mgmt.get(), "none")) {
			if (s.set_wireless_security._wep_key0) {
				supp["wep_key0"] = get_key(s.set_wireless_security._wep_key0.get(), key_type);
			}
			if (s.set_wireless_security._wep_key1) {
				supp["wep_key1"] = get_key(s.set_wireless_security._wep_key1.get(), key_type);
			}
			if (s.set_wireless_security._wep_key2) {
				supp["wep_key2"] = get_key(s.set_wireless_security._wep_key2.get(), key_type);
			}
			if (s.set_wireless_security._wep_key3) {
				supp["wep_key3"] = get_key(s.set_wireless_security._wep_key3.get(), key_type);
			}

			// wep_tx_keyidx
			if (s.set_wireless_security._wep_tx_keyidx) {
				supp["wep_tx_keyidx"] = boost::lexical_cast<std::string>(s.set_wireless_security._wep_tx_keyidx.get());
			}
		}
	}

	//
	// auth-alg
	//

	// leap
	if (s.set_wireless_security._auth_alg && boost::iequals(s.set_wireless_security._auth_alg.get(), "leap")) {
		if (s.set_wireless_security._key_mgmt || !boost::iequals(s.set_wireless_security._key_mgmt.get(), "ieee8021x")) {
			if (s.set_wireless_security._leap_username) {
				supp["identity"] = s.set_wireless_security._leap_username.get();
			}
			if (s.set_wireless_security._leap_password) {
				supp["password"] = s.set_wireless_security._leap_password.get();
			}
			supp["eap"] = "LEAP";
		}
	} else { // 8021x
		// bgscan (this option is undocumented)
		//if (boost::iequals(s.set_wireless_security._key_mgmt.get(), "wpa-eap")) {
		//	// "if WPA Enterprise, enable optimized background scanning
		//	// to ensure roaming within an ESS works well."
		//	supp["bgscan"] = "simple:30:-45:300";
		//}

		//
		// 8021x
		//
		if (boost::iequals(s.set_wireless_security._key_mgmt.get(), "ieee8021x") ||
			boost::iequals(s.set_wireless_security._key_mgmt.get(), "wpa-eap")) {
			// password
			if (s.set_8021x._password) {
				supp["password"] = s.set_8021x._password.get();
			} else if (s.set_8021x._password_raw) { // though password_raw is then mandatory
				supp["password"] = std::string(s.set_8021x._password_raw.get().begin(),
											   s.set_8021x._password_raw.get().end());
			}

			// pin
			// TODO gsm/gtc not supported

			// wired
			if (boost::iequals(s.set_con._type.get(), "802-3-ethernet")) {
				supp["key_mgmt"] = "IEEE8021X";
				supp["eapol_flags"] = "0";
				supp["ap_scan"] = "0";
			}

			if (s.set_8021x._eap) {
				std::string case_sensitive = boost::algorithm::join(s.set_8021x._eap.get(), " ");
				boost::to_upper(case_sensitive);
				supp["eap"] = case_sensitive;
			}

			// "Drop the fragment size a bit for better compatibility"
			supp["fragment_size"] = "1300";

			// phase1
			{
				std::string phase1;

				// peapver
				if (s.set_8021x._phase1_peapver) {
					phase1 += "peapver=";
					phase1 += s.set_8021x._phase1_peapver.get();
				}

				// peaplabel
				if (s.set_8021x._phase1_peaplabel) {
					if (phase1.size() > 0) {
						phase1 += ' ';
					}
					phase1 += "peaplabel=";
					phase1 += s.set_8021x._phase1_peaplabel.get();
				}

				// fast provisioning
				if (s.set_8021x._phase1_fast_provisioning) {
					if (phase1.size() > 0) {
						phase1 += ' ';
					}
					phase1 += "fast_provisioning=";
					phase1 += s.set_8021x._phase1_fast_provisioning.get();
				}

				if (phase1.size() > 0) {
					supp["phase1"] = phase1;
				}
			}

			// phase2
			{
				std::string phase2;

				// auth
				if (s.set_8021x._phase2_auth) {
					std::string case_sensitive = s.set_8021x._phase2_auth.get();
					boost::to_upper(case_sensitive);
					phase2 += "auth=";
					phase2 += case_sensitive;
				}

				// autheap
				if (s.set_8021x._phase2_autheap) {
					if (phase2.size() > 0) {
						phase2 += ' ';
					}
					std::string case_sensitive = s.set_8021x._phase2_autheap.get();
					boost::to_upper(case_sensitive);
					phase2 += "autheap=";
					phase2 += case_sensitive;
				}

				if (phase2.size()) {
					supp["phase2"] = phase2;
				}
			}

			// pac file
			if (s.set_8021x._pac_file) {
				supp["pac_file"] = s.set_8021x._pac_file.get();
			//} else {
			//	// blob not supported
			}

			// ca path
			if (s.set_8021x._ca_path) {
				supp["ca_path2"] = s.set_8021x._ca_path.get();
			}

			// phase2 ca path
			if (s.set_8021x._phase2_ca_path) {
				supp["ca_path2"] = s.set_8021x._phase2_ca_path.get();
			}

			// ca cert
			if (s.set_8021x._ca_cert) {
				std::string cert(s.set_8021x._ca_cert.get().begin(), s.set_8021x._ca_cert.get().end());
				if (boost::starts_with(cert, k_file_discriminator)) {
					cert.erase(0, k_file_discriminator.size());
					supp["ca_cert"] = cert;
				//} else {
				//	// blob scheme not supported
				}
			}

			// phase2 ca cert
			if (s.set_8021x._phase2_ca_cert) {
				std::string cert(s.set_8021x._ca_cert.get().begin(), s.set_8021x._ca_cert.get().end());
				if (boost::starts_with(cert, k_file_discriminator)) {
					cert.erase(0, k_file_discriminator.size());
					supp["ca_cert2"] = cert;
				//} else {
				//	// blob scheme not supported
				}
			}

			// subject match
			if (s.set_8021x._subject_match) {
				supp["subject_match"] = s.set_8021x._subject_match.get();
			}

			// phase2 subject match
			if (s.set_8021x._phase2_subject_match) {
				supp["subject_match2"] = s.set_8021x._phase2_subject_match.get();
			}

			// altSubjectName match
			if (s.set_8021x._altsubject_matches) {
				supp["altsubject_match"] = boost::join(s.set_8021x._altsubject_matches.get(), ";");
			}

			// phase2 altSubjectName match
			if (s.set_8021x._phase2_altsubject_matches) {
				supp["altsubject_match2"] = boost::join(s.set_8021x._phase2_altsubject_matches.get(), ";");
			}

			// private key
			if (s.set_8021x._private_key) {
				std::string key(s.set_8021x._private_key.get().begin(), s.set_8021x._private_key.get().end());
				if (boost::starts_with(key, k_file_discriminator)) {
					key.erase(0, k_file_discriminator.size());
					supp["private_key"] = key;

					if (s.set_8021x._private_key_password) {
						supp["private_key_passwd"] = s.set_8021x._private_key_password.get();
					}

					// this shouldn't be done if the format is pkcs#12, apparently
					if (s.set_8021x._client_cert) {
						supp["client_cert"] = std::string(s.set_8021x._client_cert.get().begin(),
														  s.set_8021x._client_cert.get().end());
					}
				//} else {
				//	// blob not supported
				}
			}

			// phase2 private key
			if (s.set_8021x._phase2_private_key) {
				std::string key(s.set_8021x._phase2_private_key.get().begin(), s.set_8021x._phase2_private_key.get().end());
				if (boost::starts_with(key, k_file_discriminator)) {
					key.erase(0, k_file_discriminator.size());
					supp["private_key2"] = key;

					if (s.set_8021x._phase2_private_key_password) {
						supp["private_key_passwd2"] = s.set_8021x._phase2_private_key_password.get();
					}

					// this shouldn't be done if the format is pkcs#12, apparently
					if (s.set_8021x._phase2_client_cert) {
						supp["client_cert2"] = std::string(s.set_8021x._phase2_client_cert.get().begin(),
														   s.set_8021x._phase2_client_cert.get().end());
					}
				//} else {
				//	// blob not supported
				}
			}

			// identity
			if (s.set_8021x._identity) {
				supp["identity"] = s.set_8021x._identity.get();
			}

			// anonymous identity
			if (s.set_8021x._anonymous_identity) {
				supp["anonymous_identity"] = s.set_8021x._anonymous_identity.get();
			}
		}
	}

	// psk
	// TODO: check if HEX! (.size() == 64)
	if (s.set_wireless_security._psk) {
		std::string psk = s.set_wireless_security._psk.get();
		if (psk.size() == 64) { // hex
			supp["psk"] = hex2bin(psk);
		} else/* if (psk.size() >= 8 && psk.size() < 63)*/ { // ascii
			supp["psk"] = psk;
		//} else {
		//	// invalid
		}
	}

	return supp;
}

void Connection::read_settings()
{
	log_(0, "Loading connection at \"", _file_path.generic_string(), "\"");

	connection_settings s;

	std::ifstream ifs(_file_path.generic_string());
	boost::archive::text_iarchive ia(ifs);
	ia >> s;

	_settings = s.to_map();
}

void Connection::write_settings()
{
	log_(0, "Persisting settings at \"", _file_path.generic_string(), "\"");

	connection_settings s(_settings);

	std::ofstream ofs(_file_path.generic_string());
	boost::archive::text_oarchive oa(ofs);
	oa << s;

	log_(0, "Done");
}
