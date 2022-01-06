#!/bin/sh
#
# Test CPU frequencies.
#
# author: Wu, Jheng-Jhong (goodwater.wu@gmail.com)
# version: 20220106

### CPU frequency operations ###

test_offline()
{
	local ret=0

	for cpu in $@; do
		echo "Try to set ${cpu} to offline..."
		echo "0" > /sys/devices/system/cpu/${cpu}/online
		if [ $(cat /sys/devices/system/cpu/${cpu}/online) -ne 0 ]; then
			echo "Set ${cpu} to offline failed"
			ret=1
		else
			echo "Set ${cpu} to offline OK"
			echo "1" > /sys/devices/system/cpu/${cpu}/online
		fi
	done

	return ${ret}
}

test_policy()
{
	local policy=$(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_governor)
	local ret=0

	for p in $@; do
		echo "Try to set policy to ${p}..."
		echo "${p}" > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
		if [ "$(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_governor)" != "${p}" ]; then
			echo "Set CPU policy to ${p} failed"
			ret=1
		else
			echo "Set CPU policy to ${p} OK"
		fi
	done

	echo "${policy}" > /sys/devices/system/cpu/cpufreq/policy0/scaling_governor
	return ${ret}
}

test_min_freq()
{
	local minimum=$(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq)
	local maximum=$(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_max_freq)
	local ret=0

	for min in $@; do
		if [ ${min} -le	${maximum} ]; then
			echo "Try to set minimum frequency to ${min}..."
			echo ${min} > /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq

			local min_new=$(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq)
			sleep 1
			local cur_new=$(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_cur_freq)
			if [ ${min_new} -ne ${min} -o ${cur_new} -ne ${min} ]; then
				echo "Set CPU minimum frequency to ${min} failed"
				ret=1
			else
				echo "Set CPU minimum frequency to ${min} OK"
			fi
		fi
	done

	echo ${minimum} > /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq
	return ${ret}
}

test_max_freq()
{
	local minimum=$(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq)
	local maximum=$(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_max_freq)
	local ret=0

	for max in $@; do
		if [ ${max} -ge ${minimum} ]; then
			echo "Try to set maximum frequency to ${max}..."
			echo ${max} > /sys/devices/system/cpu/cpufreq/policy0/scaling_max_freq

			local max_new=$(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_max_freq)
			for i in $(seq 100000); do
				echo "stress test" > /dev/null
			done &
			local cur_new=$(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_cur_freq)
			if [ ${max_new} -ne ${max} -o ${cur_new} -ne ${max} ]; then
				echo "Set CPU maximum frequency to ${max} failed"
				ret=1
			else
				echo "Set CPU maximum frequency to ${max} OK"
			fi
		fi
	done

	echo ${maximum} > /sys/devices/system/cpu/cpufreq/policy0/scaling_max_freq
	return ${ret}
}

show_info()
{
	local cores=$(ls /sys/devices/system/cpu/ | grep 'cpu[0-9]')
	local frequencies=$(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_available_frequencies)
	local policies=$(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_available_governors)
	local ret=0

	echo "CPU cores: $(echo "${cores}" | wc -l) ($(echo ${cores} | tr '\n' ' ' | sed 's/[ \t]*$//'))"
	echo "CPU online cores: $(cat /sys/devices/system/cpu/online)"
	echo "CPU offline cores: $(cat /sys/devices/system/cpu/offline)"
	echo "CPU frequency: $(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_cur_freq)"
	echo "CPU minimum frequency: $(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_min_freq)"
	echo "CPU maximum frequency: $(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_max_freq)"
	echo "CPU available frequencies: ${frequencies}"
	echo "CPU policy: $(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_governor)"
	echo "CPU available policies: $(cat /sys/devices/system/cpu/cpufreq/policy0/scaling_available_governors)"

	test_offline ${cores}
	if [ $? -ne 0 ]; then
		ret=1
	fi

	test_policy ${policies}
	if [ $? -ne 0 ]; then
		ret=1
	fi

	test_min_freq ${frequencies}
	if [ $? -ne 0 ]; then
		ret=1
	fi

	test_max_freq ${frequencies}
	if [ $? -ne 0 ]; then
		ret=1
	fi

	if [ ${ret} -eq 0 ]; then
		echo "Test OK!"
	else
		echo "Test failed!"
	fi
}

### Main program ###

show_info
