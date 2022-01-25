#!/bin/sh
#
# Test a USB bulk device like USB stick, disk, SSD.
# This test script must run with pattern_generator, please ensure pattern_generator exists.
#
# author: Victor Wu (victor_wu@bizlinktech.com)
# version: 20220124

### Functions ###

show_help()
{
	echo -e "\nTest a USB bulk device like USB stick, disk, SSD."
	echo -e "\nUsage: $(basename $0) [-d device] [-l loop] [-s source] [-h]"
	echo -e "\t-d\tUSB device"
	echo -e "\t-l\tloop time"
	echo -e "\t-s\tTest source file"
	echo -e "\t-h\tshow this help\n"
}

### Main program ###

test_dir="/tmp/usb_bulk"
device="/dev/sda1"
loop=10
src="hex_pattern"

options="d:l:s:h"
while getopts ${options} opt
do
	case ${opt} in
	d)
		device=${OPTARG};;
	l)
		loop=${OPTARG};;
	s)
		src=${OPTARG};;
	*)
		show_help $0
		exit 0;;
	esac
done

if [ ! -e ${device} ] || [ ${loop} -lt 1 ]; then
	show_help $0
	exit 1
fi

mkdir -p ${test_dir}
mount ${device} ${test_dir} 2>/dev/null
echo -n "Generate a pattern file..."
rm -f ${src}
pattern_generator -f ${src} -s 104857600
echo "OK"

for i in $(seq ${loop}); do
	src_size=$(stat -c %s ${src})

	echo "Loop...$i"
	dd if=${src} of=${test_dir}/$(basename ${src}).w conv=sync,noerror
	cmp ${src} ${test_dir}/$(basename ${src}).w
	if [ $? -ne 0 ]; then
		echo "Write test failed!"
		exit 1
	fi

	dd if=${test_dir}/$(basename ${src}).w of=${src}.r conv=sync,noerror
	cmp ${src} ${src}.r
	if [ $? -ne 0 ]; then
		echo "Read test failed!"
		exit 1
	fi

	rm -f ${src}.r ${test_dir}/$(basename ${src}).w
done

umount ${test_dir}
echo "Test OK!"
