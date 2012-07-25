
pgrep wpa_supplicant &> /dev/null && sudo killall wpa_supplicant
pgrep dhclient       &> /dev/null && sudo killall dhclient

sudo gnome-terminal \
		--tab -e "                              wpa_supplicant -u" \
		--tab -e "sh -c 'sleep 0 && gdb -ex run ./odtone-mihf'" \
		--tab -e "sh -c 'sleep 1 && gdb -ex run ./sap_80211'" \
		--tab -e "sh -c 'sleep 1 && gdb -ex run ./sap_8023'" \
		--tab -e "sh -c 'sleep 3 && gdb -ex run ./networkmanager'"


