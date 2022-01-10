#!/bin/sh
#
# Test LEDs.
#
# author: Victor Wu (victor_wu@bizlinktech.com)
# version: 20220110

### Main program ###

leds=$(ls /sys/class/leds/)

for led in ${leds}; do
	max_brightness=$(cat /sys/class/leds/${led}/max_brightness)
	brightness=$(cat /sys/class/leds/${led}/brightness)
	echo "----- ${led} -----"
	echo "Maxinum brightness: ${max_brightness}"
	echo "Available trigger sources: $(cat /sys/class/leds/${led}/trigger)"
	for i in $(seq 10); do
		printf "\rPlease check if ${led} is blinking...%02d" "${i}"
		if [ $((i & 1)) -eq 1 ]; then
			echo ${max_brightness} > /sys/class/leds/${led}/brightness
		else
			echo 0 > /sys/class/leds/${led}/brightness
		fi
		sleep 1
		echo ${brightness} > /sys/class/leds/${led}/brightness
	done
	echo "" # new line
done

echo "Test finished!"
