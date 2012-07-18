
sudo gnome-terminal \
		--tab -e "                              wpa_supplicant -u" \
		--tab -e "                              dhcpcd -B" \
		--tab -e "sh -c 'sleep 0 && gdb -ex run ./odtone-mihf'" \
		--tab -e "sh -c 'sleep 1 && gdb -ex run ./sap_80211'" \
		--tab -e "sh -c 'sleep 2 && gdb -ex run ./networkmanager'"
#		--tab -e "sh -c 'sleep 1 && gdb -ex run ./sap_8023'" \
#       --tab -e "                              dhcpcd -B -t 0 wlan0" \
