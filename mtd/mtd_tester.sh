#!/bin/sh
#
# Test MTD read/write/erase.
#
# author: Victor Wu (victor_wu@bizlinktech.com)
# version: 20220121

### Test functions ###

show_help()
{
	echo -e "\nTest MTD read/write/erase"
	echo -e "\nUsage: $(basename $0) <-d device> [-l loop_time] [-h]"
	echo -e "\t-d\tmtd device"
	echo -e "\t-l\tloop time"
	echo -e "\t-h\tshow this help\n"
}

test_mtd()
{
	local mtd_size=$(mtd_debug info $1 | grep 'mtd\.size' | awk '{ print $3 }')
	local mtd_erasesize=$(mtd_debug info $1 | grep 'mtd\.erasesize' | awk '{ print $3 }')
	local max_test_size=$((mtd_erasesize * 10))

	for i in $(seq $2); do
		local remain=${mtd_size}
		local offset=0

		echo "Loop...$i"
		while [ ${remain} -gt 0 ]; do
			if [ ${max_test_size} -le ${remain} ]; then
				local test_size=${max_test_size}
			else
				local test_size=${remain}
			fi

			mtd_debug read $1 ${offset} ${test_size} /tmp/$(basename $1)
			mtd_debug erase $1 ${offset} ${test_size}
			mtd_debug write $1 ${offset} ${test_size} /tmp/$(basename $1)
			mtd_debug read $1 ${offset} ${test_size} /tmp/$(basename $1)-2
			cmp /tmp/$(basename $1) /tmp/$(basename $1)-2
			if [ $? -ne 0 ]; then
				echo "$1 rw test failed - offset: ${offset} rw size: ${test_size}"
				return 1
			fi

			offset=$((offset + test_size))
			remain=$((remain - test_size))
		done
	done

	rm -f /tmp/$(basename $1) > /dev/null
	rm -f /tmp/$(basename $1)-2 > /dev/null
}

### Main program ###

loop=1

options="d:l:h"
while getopts ${options} opt
do
	case ${opt} in
	d)
		dev=${OPTARG};;
	l)
		loop=${OPTARG}
		if [ ${loop} -lt 1 ]; then
			echo "Loop time must >= 1"
			exit 1
		fi
		;;
	*)
		show_help $0
		exit 0;;
	esac
done

if [ -n "${dev}" ]; then
	test_mtd ${dev} ${loop}
	if [ $? -eq 0 ]; then
		echo "Test OK!"
	else
		echo "Test failed!"
	fi
else
	show_help $0
	exit 1
fi
