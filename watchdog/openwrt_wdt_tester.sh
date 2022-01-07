#!/bin/sh
#
# Test OpenWRT watchdog.
# This test script must run with wdt_timeout, please ensure wdt_timeout exists.
#
# author: Wu, Jheng-Jhong (goodwater.wu@gmail.com)
# version: 20220107

### Watchdog operations ###

start_watchdog()
{
	ubus call system watchdog "{ \"stop\": false }" > /dev/null
}

stop_watchdog()
{
	ubus call system watchdog "{ \"stop\": true }" > /dev/null
}

set_timeout()
{
	ubus call system watchdog "{ \"timeout\": $1 }" > /dev/null
}

start_deadlock()
{
	./wdt_timeout $1
}

stop_deadlock()
{
	ps | grep 'wdt_timeout' | grep -v 'grep' | awk '{ print $1 }' | xargs kill
}

test_watchdog()
{
	local cores=$(ls /sys/devices/system/cpu/ | grep 'cpu[0-9]' | wc -l)

	stop_watchdog
	set_timeout ${timeout}
	start_deadlock ${cores}&
	sleep 1
	echo -n "Wait for timeout..."
	for i in $(seq $((timeout - 1)) -1 0); do
		printf "\rWait for timeout...%02d" ${i}
		sleep 1
	done

	sleep 5
	stop_deadlock
	start_watchdog
	return 1
}

### Main program ###

timeout=$(ubus call system watchdog | grep timeout | awk '{ print $2 }')

if [ $# -ge 1 ]; then
	timeout=$1
fi

test_watchdog ${timeout}
if [ $? -ne 0 ]; then
	echo "Test failed!"
fi
