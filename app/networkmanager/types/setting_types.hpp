
#include <boost/serialization/vector.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/map.hpp>

// make struct serializable
#include <dbus-c++/types.h>
namespace boost {
namespace serialization {
	template <class Archive,
		class T1,
		class T2,
		class T3,
		class T4,
		class T5,
		class T6,
		class T7,
		class T8,
		class T9,
		class T10,
		class T11,
		class T12,
		class T13,
		class T14,
		class T15,
		class T16>
	void serialize(Archive &ar, DBus::Struct<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16> &s, const unsigned int version)
	{
		ar & s._1;
		ar & s._2;
		ar & s._3;
		ar & s._4;
		ar & s._5;
		ar & s._6;
		ar & s._7;
		ar & s._8;
		ar & s._9;
		ar & s._10;
		ar & s._11;
		ar & s._12;
		ar & s._13;
		ar & s._14;
		ar & s._15;
		ar & s._16;
	}

	template <class Archive,
		class T1,
		class T2,
		class T3>
	void serialize(Archive &ar, DBus::Struct<T1, T2, T3> &s, const unsigned int version)
	{
		ar & s._1;
		ar & s._2;
		ar & s._3;
	}

	template <class Archive,
		class T1,
		class T2,
		class T3,
		class T4>
	void serialize(Archive &ar, DBus::Struct<T1, T2, T3, T4> &s, const unsigned int version)
	{
		ar & s._1;
		ar & s._2;
		ar & s._3;
		ar & s._4;
	}

}; };

namespace odtone {
namespace networkmanager {
	typedef std::map<std::string, std::map<std::string, ::DBus::Variant>> settings_map;
	typedef std::map<std::string, ::DBus::Variant>                        setting_pairs;

	template <class T>
	static void setting_value(const setting_pairs &s, const char *key, boost::optional<T> &value)
	{
		if (s.find(key) != s.end()) {
			DBus::MessageIter it = s.find(key)->second.reader();
			T v;
			it >> v;
			value = v;
		}
	}

	template <class T>
	static void map_value(setting_pairs &s, const char *key, const boost::optional<T> &value)
	{
		if (value) {
			DBus::Variant v;
			DBus::MessageIter it = v.writer();
			it << value.get();
			s[key] = v;
		}
	}

	struct settings_connection {
		static constexpr const char *key = "connection";

		settings_connection ()
		{
		}

		settings_connection (const setting_pairs &p)
		{
			from_map(p);
		}

		template<class Archive>
		void serialize(Archive &ar, const unsigned int version)
		{
			ar & _name;
			ar & _id;
			ar & _uuid;
			ar & _type;
			ar & _permissions;
			ar & _autoconnect;
			ar & _timestamp;
			ar & _read_only;
			ar & _zone;
			ar & _master;
			ar & _slave_type;
		}

		void from_map(const setting_pairs &p) {
			setting_value(p, "id", _id);
			setting_value(p, "uuid", _uuid);
			setting_value(p, "type", _type);
			setting_value(p, "permissions", _permissions);
			setting_value(p, "autoconnect", _autoconnect);
			setting_value(p, "timestamp", _timestamp);
			setting_value(p, "read-only", _read_only);
			setting_value(p, "zone", _zone);
			setting_value(p, "master", _master);
			setting_value(p, "slave-type", _slave_type);
		};

		setting_pairs to_map()
		{
			setting_pairs p;

			map_value(p, "id", _id);
			map_value(p, "uuid", _uuid);
			map_value(p, "type", _type);
			map_value(p, "permissions", _permissions);
			map_value(p, "autoconnect", _autoconnect);
			map_value(p, "timestamp", _timestamp);
			map_value(p, "read-only", _read_only);
			map_value(p, "zone", _zone);
			map_value(p, "master", _master);
			map_value(p, "slave-type", _slave_type);

			return p;
		}

		std::string                               _name = "connection";
		boost::optional<std::string>              _id;
		boost::optional<std::string>              _uuid;
		boost::optional<std::string>              _type;
		boost::optional<std::vector<std::string>> _permissions;
		boost::optional<bool>                     _autoconnect;
		boost::optional<uint64_t>                 _timestamp;
		boost::optional<bool>                     _read_only;
		boost::optional<std::string>              _zone;
		boost::optional<std::string>              _master;
		boost::optional<std::string>              _slave_type;
	};

	struct settings_ipv4 {
		static constexpr const char *key = "ipv4";

		settings_ipv4 ()
		{
		}

		settings_ipv4 (const setting_pairs &p)
		{
			from_map(p);
		}

		template<class Archive>
		void serialize(Archive &ar, const unsigned int version)
		{
			ar & _name;
			ar & _method;
			ar & _dns;
			ar & _dns_search;
			ar & _addresses;
			ar & _routes;
			ar & _ignore_auto_routes;
			ar & _ignore_auto_dns;
			ar & _dhcp_client_id;
			ar & _dhcp_send_hostname;
			ar & _dhcp_hostname;
			ar & _never_default;
			ar & _may_fail;
		}

		void from_map(const setting_pairs &p) {
			setting_value(p, "method", _method);
			setting_value(p, "dns", _dns);
			setting_value(p, "dns-search", _dns_search);
			setting_value(p, "addresses", _addresses);
			setting_value(p, "routes", _routes);
			setting_value(p, "ignore-auto-routes", _ignore_auto_routes);
			setting_value(p, "ignore-auto-dns", _ignore_auto_dns);
			setting_value(p, "dhcp-client-id", _dhcp_client_id);
			setting_value(p, "dhcp-send-hostname", _dhcp_send_hostname);
			setting_value(p, "dhcp-hostname", _dhcp_hostname);
			setting_value(p, "never-default", _never_default);
			setting_value(p, "may-fail", _may_fail);
		};

		setting_pairs to_map()
		{
			setting_pairs p;

			map_value(p, "method", _method);
			map_value(p, "dns", _dns);
			map_value(p, "dns-search", _dns_search);
			map_value(p, "addresses", _addresses);
			map_value(p, "routes", _routes);
			map_value(p, "ignore-auto-routes", _ignore_auto_routes);
			map_value(p, "ignore-auto-dns", _ignore_auto_dns);
			map_value(p, "dhcp-client-id", _dhcp_client_id);
			map_value(p, "dhcp-send-hostname", _dhcp_send_hostname);
			map_value(p, "dhcp-hostname", _dhcp_hostname);
			map_value(p, "never-default", _never_default);
			map_value(p, "may-fail", _may_fail);

			return p;
		}

		std::string                                         _name = "ipv4";
		boost::optional<std::string>                        _method;
		boost::optional<std::vector<uint32_t>>              _dns;
		boost::optional<std::vector<std::string>>           _dns_search;
		boost::optional<std::vector<std::vector<uint32_t>>> _addresses;
		boost::optional<std::vector<std::vector<uint32_t>>> _routes;
		boost::optional<bool>                               _ignore_auto_routes;
		boost::optional<bool>                               _ignore_auto_dns;
		boost::optional<std::string>                        _dhcp_client_id;
		boost::optional<bool>                               _dhcp_send_hostname;
		boost::optional<std::string>                        _dhcp_hostname;
		boost::optional<bool>                               _never_default;
		boost::optional<bool>                               _may_fail;
	};

	struct settings_ipv6 {
		static constexpr const char *key = "ipv6";

		settings_ipv6 ()
		{
		}

		settings_ipv6 (const setting_pairs &p)
		{
			from_map(p);
		}

		template<class Archive>
		void serialize(Archive &ar, const unsigned int version)
		{
			ar & _name;
			ar & _method;
			ar & _dns;
			ar & _dns_search;
			ar & _addresses;
			ar & _routes;
			ar & _ignore_auto_routes;
			ar & _ignore_auto_dns;
			ar & _never_default;
			ar & _may_fail;
			ar & _ip6_privacy;
		}

		void from_map(const setting_pairs &p) {
			setting_value(p, "method", _method);
			setting_value(p, "dns", _dns);
			setting_value(p, "dns-search", _dns_search);
			setting_value(p, "addresses", _addresses);
			setting_value(p, "routes", _routes);
			setting_value(p, "ignore-auto_routes", _ignore_auto_routes);
			setting_value(p, "ignore-auto_dns", _ignore_auto_dns);
			setting_value(p, "never-default", _never_default);
			setting_value(p, "may-fail", _may_fail);
			setting_value(p, "ip6-privacy", _ip6_privacy);
		};

		setting_pairs to_map()
		{
			setting_pairs p;

			map_value(p, "method", _method);
			map_value(p, "dns", _dns);
			map_value(p, "dns-search", _dns_search);
			map_value(p, "addresses", _addresses);
			map_value(p, "routes", _routes);
			map_value(p, "ignore-auto_routes", _ignore_auto_routes);
			map_value(p, "ignore-auto_dns", _ignore_auto_dns);
			map_value(p, "never-default", _never_default);
			map_value(p, "may-fail", _may_fail);
			map_value(p, "ip6-privacy", _ip6_privacy);

			return p;
		}

		std::string                                              _name = "ipv6";
		boost::optional<std::string>                             _method;
		boost::optional<std::vector<std::vector<unsigned char>>> _dns;
		boost::optional<std::vector<std::string>>                _dns_search;
		boost::optional<bool>                                    _ignore_auto_routes;
		boost::optional<bool>                                    _ignore_auto_dns;
		boost::optional<bool>                                    _never_default;
		boost::optional<bool>                                    _may_fail;
		boost::optional<int32_t>                                 _ip6_privacy;
		boost::optional<std::vector<DBus::Struct<
			std::vector<unsigned char>,
			uint32_t,
			std::vector<unsigned char>>>>                        _addresses;
		boost::optional<std::vector<DBus::Struct<
			std::vector<unsigned char>,
			uint32_t,
			std::vector<unsigned char>,
			uint32_t>>>                                          _routes;
	};

	struct settings_wimax {
		static constexpr const char *key = "wimax";

		settings_wimax ()
		{
		}

		settings_wimax (const setting_pairs &p)
		{
			from_map(p);
		}

		template<class Archive>
		void serialize(Archive &ar, const unsigned int version)
		{
			ar & _name;
			ar & _network_name;
			ar & _mac_address;
		}

		void from_map(const setting_pairs &p) {
			setting_value(p, "network-name", _network_name);
			setting_value(p, "mac-address", _mac_address);
		};

		setting_pairs to_map()
		{
			setting_pairs p;

			map_value(p, "network-name", _network_name);
			map_value(p, "mac-address", _mac_address);

			return p;
		}

		std::string                                 _name = "wimax";
		boost::optional<std::string>                _network_name;
		boost::optional<std::vector<unsigned char>> _mac_address;
	};

	struct settings_8021x {
		static constexpr const char *key = "802-1x";

		settings_8021x ()
		{
		}

		settings_8021x (const setting_pairs &p)
		{
			from_map(p);
		}

		template<class Archive>
		void serialize(Archive &ar, const unsigned int version)
		{
			ar & _name;
			ar & _eap;
			ar & _identity;
			ar & _anonymous_identity;
			ar & _pac_file;
			ar & _ca_cert;
			ar & _ca_path;
			ar & _subject_match;
			ar & _altsubject_matches;
			ar & _client_cert;
			ar & _phase1_peapver;
			ar & _phase1_peaplabel;
			ar & _phase1_fast_provisioning;
			ar & _phase2_auth;
			ar & _phase2_autheap;
			ar & _phase2_ca_cert;
			ar & _phase2_ca_path;
			ar & _phase2_subject_match;
			ar & _phase2_altsubject_matches;
			ar & _phase2_client_cert;
			ar & _password;
			ar & _password_flags;
			ar & _password_raw;
			ar & _password_raw_flags;
			ar & _private_key;
			ar & _private_key_password;
			ar & _private_key_password_flags;
			ar & _phase2_private_key;
			ar & _phase2_private_key_password;
			ar & _phase2_private_key_password_flags;
			ar & _system_ca_certs;
		}

		void from_map(const setting_pairs &p) {
			setting_value(p, "eap", _eap);
			setting_value(p, "identity", _identity);
			setting_value(p, "anonymous-identity", _anonymous_identity);
			setting_value(p, "pac-file", _pac_file);
			setting_value(p, "ca-cert", _ca_cert);
			setting_value(p, "ca-path", _ca_path);
			setting_value(p, "subject-match", _subject_match);
			setting_value(p, "altsubject-matches", _altsubject_matches);
			setting_value(p, "client-cert", _client_cert);
			setting_value(p, "phase1-peapver", _phase1_peapver);
			setting_value(p, "phase1-peaplabel", _phase1_peaplabel);
			setting_value(p, "phase1-fast-provisioning", _phase1_fast_provisioning);
			setting_value(p, "phase2-auth", _phase2_auth);
			setting_value(p, "phase2-autheap", _phase2_autheap);
			setting_value(p, "phase2-ca-cert", _phase2_ca_cert);
			setting_value(p, "phase2-ca-path", _phase2_ca_path);
			setting_value(p, "phase2-subject-match", _phase2_subject_match);
			setting_value(p, "phase2-altsubject-matches", _phase2_altsubject_matches);
			setting_value(p, "phase2-client-cert", _phase2_client_cert);
			setting_value(p, "password", _password);
			setting_value(p, "password-flags", _password_flags);
			setting_value(p, "password-raw", _password_raw);
			setting_value(p, "password-raw-flags", _password_raw_flags);
			setting_value(p, "private-key", _private_key);
			setting_value(p, "private-key-password", _private_key_password);
			setting_value(p, "private-key-password-flags", _private_key_password_flags);
			setting_value(p, "phase2-private-key", _phase2_private_key);
			setting_value(p, "phase2-private-key-password", _phase2_private_key_password);
			setting_value(p, "phase2-private-key-password-flags", _phase2_private_key_password_flags);
			setting_value(p, "system-ca-certs", _system_ca_certs);

		};

		setting_pairs to_map()
		{
			setting_pairs p;

			map_value(p, "eap", _eap);
			map_value(p, "identity", _identity);
			map_value(p, "anonymous-identity", _anonymous_identity);
			map_value(p, "pac-file", _pac_file);
			map_value(p, "ca-cert", _ca_cert);
			map_value(p, "ca-path", _ca_path);
			map_value(p, "subject-match", _subject_match);
			map_value(p, "altsubject-matches", _altsubject_matches);
			map_value(p, "client-cert", _client_cert);
			map_value(p, "phase1-peapver", _phase1_peapver);
			map_value(p, "phase1-peaplabel", _phase1_peaplabel);
			map_value(p, "phase1-fast-provisioning", _phase1_fast_provisioning);
			map_value(p, "phase2-auth", _phase2_auth);
			map_value(p, "phase2-autheap", _phase2_autheap);
			map_value(p, "phase2-ca-cert", _phase2_ca_cert);
			map_value(p, "phase2-ca-path", _phase2_ca_path);
			map_value(p, "phase2-subject-match", _phase2_subject_match);
			map_value(p, "phase2-altsubject-matches", _phase2_altsubject_matches);
			map_value(p, "phase2-client-cert", _phase2_client_cert);
			map_value(p, "password", _password);
			map_value(p, "password-flags", _password_flags);
			map_value(p, "password-raw", _password_raw);
			map_value(p, "password-raw-flags", _password_raw_flags);
			map_value(p, "private-key", _private_key);
			map_value(p, "private-key-password", _private_key_password);
			map_value(p, "private-key-password-flags", _private_key_password_flags);
			map_value(p, "phase2-private-key", _phase2_private_key);
			map_value(p, "phase2-private-key-password", _phase2_private_key_password);
			map_value(p, "phase2-private-key-password-flags", _phase2_private_key_password_flags);
			map_value(p, "system-ca-certs", _system_ca_certs);

			return p;
		}

		std::string                                 _name = "802-1x";
		boost::optional<std::vector<std::string>>   _eap;
		boost::optional<std::string>                _identity;
		boost::optional<std::string>                _anonymous_identity;
		boost::optional<std::string>                _pac_file;
		boost::optional<std::vector<unsigned char>> _ca_cert;
		boost::optional<std::string>                _ca_path;
		boost::optional<std::string>                _subject_match;
		boost::optional<std::vector<std::string>>   _altsubject_matches;
		boost::optional<std::vector<unsigned char>> _client_cert;
		boost::optional<std::string>                _phase1_peapver;
		boost::optional<std::string>                _phase1_peaplabel;
		boost::optional<std::string>                _phase1_fast_provisioning;
		boost::optional<std::string>                _phase2_auth;
		boost::optional<std::string>                _phase2_autheap;
		boost::optional<std::vector<unsigned char>> _phase2_ca_cert;
		boost::optional<std::string>                _phase2_ca_path;
		boost::optional<std::string>                _phase2_subject_match;
		boost::optional<std::vector<std::string>>   _phase2_altsubject_matches;
		boost::optional<std::vector<unsigned char>> _phase2_client_cert;
		boost::optional<std::string>                _password;
		boost::optional<uint32_t>                   _password_flags;
		boost::optional<std::vector<unsigned char>> _password_raw;
		boost::optional<uint32_t>                   _password_raw_flags;
		boost::optional<std::vector<unsigned char>> _private_key;
		boost::optional<std::string>                _private_key_password;
		boost::optional<uint32_t>                   _private_key_password_flags;
		boost::optional<std::vector<unsigned char>> _phase2_private_key;
		boost::optional<std::string>                _phase2_private_key_password;
		boost::optional<uint32_t>                   _phase2_private_key_password_flags;
		boost::optional<bool>                       _system_ca_certs;
	};

	struct settings_ethernet {
		static constexpr const char *key = "802-3-ethernet";

		settings_ethernet ()
		{
		}

		settings_ethernet (const setting_pairs &p)
		{
			from_map(p);
		}

		template<class Archive>
		void serialize(Archive &ar, const unsigned int version)
		{
			ar & _name;
			ar & _port;
			ar & _speed;
			ar & _duplex;
			ar & _auto_negotiate;
			ar & _mac_address;
			ar & _cloned_mac_address;
			ar & _mac_address_blacklist;
			ar & _mtu;
			ar & _s390_subchannels;
			ar & _s390_nettype;
			ar & _s390_options;
		}

		void from_map(const setting_pairs &p) {
			setting_value(p, "port", _port);
			setting_value(p, "speed", _speed);
			setting_value(p, "duplex", _duplex);
			setting_value(p, "auto-negotiate", _auto_negotiate);
			setting_value(p, "mac-address", _mac_address);
			setting_value(p, "cloned-mac-address", _cloned_mac_address);
			setting_value(p, "mac-address-blacklist", _mac_address_blacklist);
			setting_value(p, "mtu", _mtu);
			setting_value(p, "s390-subchannels", _s390_subchannels);
			setting_value(p, "s390-nettype", _s390_nettype);
			setting_value(p, "s390-options", _s390_options);
		};

		setting_pairs to_map()
		{
			setting_pairs p;

			map_value(p, "port", _port);
			map_value(p, "speed", _speed);
			map_value(p, "duplex", _duplex);
			map_value(p, "auto-negotiate", _auto_negotiate);
			map_value(p, "mac-address", _mac_address);
			map_value(p, "cloned-mac-address", _cloned_mac_address);
			map_value(p, "mac_address-blacklist", _mac_address_blacklist);
			map_value(p, "mtu", _mtu);
			map_value(p, "s390-subchannels", _s390_subchannels);
			map_value(p, "s390-nettype", _s390_nettype);
			map_value(p, "s390-options", _s390_options);

			return p;
		}

		std::string                                         _name = "802-3-ethernet";
		boost::optional<std::string>                        _port;
		boost::optional<uint32_t>                           _speed;
		boost::optional<std::string>                        _duplex;
		boost::optional<bool>                               _auto_negotiate;
		boost::optional<std::vector<unsigned char>>         _mac_address;
		boost::optional<std::vector<unsigned char>>         _cloned_mac_address;
		boost::optional<std::vector<std::string>>           _mac_address_blacklist;
		boost::optional<uint32_t>                           _mtu;
		boost::optional<std::vector<std::string>>           _s390_subchannels;
		boost::optional<std::string>                        _s390_nettype;
		boost::optional<std::map<std::string, std::string>> _s390_options;
	};

	struct settings_wireless {
		static constexpr const char *key = "802-11-wireless";

		settings_wireless ()
		{
		}

		settings_wireless (const setting_pairs &p)
		{
			from_map(p);
		}

		template<class Archive>
		void serialize(Archive &ar, const unsigned int version)
		{
			ar & _name;
			ar & _ssid;
			ar & _mode;
			ar & _band;
			ar & _channel;
			ar & _bssid;
			ar & _rate;
			ar & _tx_power;
			ar & _mac_address;
			ar & _cloned_mac_address;
			ar & _mac_address_blacklist;
			ar & _mtu;
			ar & _seen_bssids;
			ar & _security;
		}

		void from_map(const setting_pairs &p) {
			setting_value(p, "ssid", _ssid);
			setting_value(p, "mode", _mode);
			setting_value(p, "band", _band);
			setting_value(p, "channel", _channel);
			setting_value(p, "bssid", _bssid);
			setting_value(p, "rate", _rate);
			setting_value(p, "tx-power", _tx_power);
			setting_value(p, "mac-address", _mac_address);
			setting_value(p, "cloned-mac-address", _cloned_mac_address);
			setting_value(p, "mac-address-blacklist", _mac_address_blacklist);
			setting_value(p, "mtu", _mtu);
			setting_value(p, "seen-bssids", _seen_bssids);
			setting_value(p, "security", _security);
		};

		setting_pairs to_map()
		{
			setting_pairs p;

			map_value(p, "ssid", _ssid);
			map_value(p, "mode", _mode);
			map_value(p, "band", _band);
			map_value(p, "channel", _channel);
			map_value(p, "bssid", _bssid);
			map_value(p, "rate", _rate);
			map_value(p, "tx-power", _tx_power);
			map_value(p, "mac-address", _mac_address);
			map_value(p, "cloned-mac-address", _cloned_mac_address);
			map_value(p, "mac-address-blacklist", _mac_address_blacklist);
			map_value(p, "mtu", _mtu);
			map_value(p, "seen-bssids", _seen_bssids);
			map_value(p, "security", _security);

			return p;
		}

		std::string                                         _name = "802-11-wireless";
		boost::optional<std::vector<unsigned char>>         _ssid;
		boost::optional<std::string>                        _mode;
		boost::optional<std::string>                        _band;
		boost::optional<uint32_t>                           _channel;
		boost::optional<std::vector<unsigned char>>         _bssid;
		boost::optional<uint32_t>                           _rate;
		boost::optional<uint32_t>                           _tx_power;
		boost::optional<std::vector<unsigned char>>         _mac_address;
		boost::optional<std::vector<unsigned char>>         _cloned_mac_address;
		boost::optional<std::vector<std::string>>           _mac_address_blacklist;
		boost::optional<uint32_t>                           _mtu;
		boost::optional<std::vector<std::string>>           _seen_bssids;
		boost::optional<std::string>                        _security;
	};

	struct settings_wireless_security {
		static constexpr const char *key = "802-11-wireless-security";

		settings_wireless_security ()
		{
		}

		settings_wireless_security (const setting_pairs &p)
		{
			from_map(p);
		}

		template<class Archive>
		void serialize(Archive &ar, const unsigned int version)
		{
			ar & _name;
			ar & _key_mgmt;
			ar & _wep_tx_keyidx;
			ar & _auth_alg;
			ar & _proto;
			ar & _pairwise;
			ar & _group;
			ar & _leap_username;
			ar & _wep_key0;
			ar & _wep_key1;
			ar & _wep_key2;
			ar & _wep_key3;
			ar & _wep_key_flags;
			ar & _wep_key_type;
			ar & _psk;
			ar & _psk_flags;
			ar & _leap_password;
			ar & _leap_password_flags;
		}

		void from_map(const setting_pairs &p) {
			setting_value(p, "key-mgmt", _key_mgmt);
			setting_value(p, "wep-tx-keyidx", _wep_tx_keyidx);
			setting_value(p, "auth-alg", _auth_alg);
			setting_value(p, "proto", _proto);
			setting_value(p, "pairwise", _pairwise);
			setting_value(p, "group", _group);
			setting_value(p, "leap-username", _leap_username);
			setting_value(p, "wep-key0", _wep_key0);
			setting_value(p, "wep-key1", _wep_key1);
			setting_value(p, "wep-key2", _wep_key2);
			setting_value(p, "wep-key3", _wep_key3);
			setting_value(p, "wep-key-flags", _wep_key_flags);
			setting_value(p, "wep-key-type", _wep_key_type);
			setting_value(p, "psk", _psk);
			setting_value(p, "psk-flags", _psk_flags);
			setting_value(p, "leap-password", _leap_password);
			setting_value(p, "leap-password-flags", _leap_password_flags);
		};

		setting_pairs to_map()
		{
			setting_pairs p;

			map_value(p, "key-mgmt", _key_mgmt);
			map_value(p, "wep-tx-keyidx", _wep_tx_keyidx);
			map_value(p, "auth-alg", _auth_alg);
			map_value(p, "proto", _proto);
			map_value(p, "pairwise", _pairwise);
			map_value(p, "group", _group);
			map_value(p, "leap-username", _leap_username);
			map_value(p, "wep-key0", _wep_key0);
			map_value(p, "wep-key1", _wep_key1);
			map_value(p, "wep-key2", _wep_key2);
			map_value(p, "wep-key3", _wep_key3);
			map_value(p, "wep-key-flags", _wep_key_flags);
			map_value(p, "wep-key-type", _wep_key_type);
			map_value(p, "psk", _psk);
			map_value(p, "psk-flags", _psk_flags);
			map_value(p, "leap-password", _leap_password);
			map_value(p, "leap-password-flags", _leap_password_flags);

			return p;
		}

		std::string                                         _name = "802-11-wireless-security";
		boost::optional<std::string>                        _key_mgmt;
		boost::optional<uint32_t>                           _wep_tx_keyidx;
		boost::optional<std::string>                        _auth_alg;
		boost::optional<std::vector<std::string>>           _proto;
		boost::optional<std::vector<std::string>>           _pairwise;
		boost::optional<std::vector<std::string>>           _group;
		boost::optional<std::string>                        _leap_username;
		boost::optional<std::string>                        _wep_key0;
		boost::optional<std::string>                        _wep_key1;
		boost::optional<std::string>                        _wep_key2;
		boost::optional<std::string>                        _wep_key3;
		boost::optional<uint32_t>                           _wep_key_flags;
		boost::optional<uint32_t>                           _wep_key_type;
		boost::optional<std::string>                        _psk;
		boost::optional<uint32_t>                           _psk_flags;
		boost::optional<std::string>                        _leap_password;
		boost::optional<uint32_t>                           _leap_password_flags;
	};

	struct connection_settings {
		connection_settings ()
		{
		}

		connection_settings (settings_map &m)
		{
			from_map(m);
		}

		template<class Archive>
		void serialize(Archive &ar, const unsigned int version)
		{
			ar & set_con;
			ar & set_ipv4;
			ar & set_ipv6;
			ar & set_wimax;
			ar & set_ethernet;
			ar & set_wireless;
			ar & set_wireless_security;
			ar & set_8021x;
		}

		void from_map(settings_map &m)
		{
			if (m.find(settings_connection::key) != m.end()) {
				set_con = settings_connection(m[settings_connection::key]);
			}
			if (m.find(settings_ipv4::key) != m.end()) {
				set_ipv4 = settings_ipv4(m[settings_ipv4::key]);
			}
			if (m.find(settings_ipv6::key) != m.end()) {
				set_ipv6 = settings_ipv6(m[settings_ipv6::key]);
			}
			if (m.find(settings_ethernet::key) != m.end()) {
				set_ethernet = settings_ethernet(m[settings_ethernet::key]);
			}
			if (m.find(settings_wireless::key) != m.end()) {
				set_wireless = settings_wireless(m[settings_wireless::key]);
			}
			if (m.find(settings_wireless_security::key) != m.end()) {
				set_wireless_security = settings_wireless_security(m[settings_wireless_security::key]);
			}
			if (m.find(settings_8021x::key) != m.end()) {
				set_8021x.from_map(m[settings_8021x::key]);
			}
		};

		settings_map to_map()
		{
			settings_map m;
			setting_pairs s;

			s = set_con.to_map();
			if (s.size() > 0) {
				m[settings_connection::key] = set_con.to_map();
			}

			s = set_ipv4.to_map();
			if (s.size() > 0) {
				m[settings_ipv4::key] = set_ipv4.to_map();
			}

			s = set_ipv6.to_map();
			if (s.size() > 0) {
				m[settings_ipv6::key] = set_ipv6.to_map();
			}

			s = set_ethernet.to_map();
			if (s.size() > 0) {
				m[settings_ethernet::key] = s;
			}

			s = set_wireless.to_map();
			if (s.size() > 0) {
				m[settings_wireless::key] = s;
			}

			s = set_wireless_security.to_map();
			if (s.size() > 0) {
				m[settings_wireless_security::key] = s;
			}

			s = set_wimax.to_map();
			if (s.size() > 0) {
				m[settings_wimax::key] = s;
			}

			s = set_8021x.to_map();
			if (s.size() > 0) {
				m[settings_8021x::key] = s;
			}

			return m;
		}

		settings_connection          set_con;
		settings_ipv4                set_ipv4;
		settings_ipv6                set_ipv6;
		settings_wimax               set_wimax;
		settings_ethernet            set_ethernet;
		settings_wireless            set_wireless;
		settings_wireless_security   set_wireless_security;
		settings_8021x               set_8021x;
	};

};
};
