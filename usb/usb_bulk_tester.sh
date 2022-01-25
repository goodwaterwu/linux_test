#!/bin/sh
#
# Test a USB bulk device like USB stick, disk, SSD.
#
# author: Victor Wu (goodwater.wu@gmail.com)
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

if [ ! -f ${device} ] || [ ${loop} -lt 1 ] || [ ! -f ${src} ]; then
	show_help $0
fi

mkdir -p ${test_dir}
mount ${device} ${test_dir} 2>/dev/null
for i in $(seq ${loop}); do
	src_size=$(stat -c %s ${src})

	echo "loop...${i}"
	dd if=${src} of=${test_dir}/$(basename ${src}) conv=sync,noerror
	dd if=${test_dir}/$(basename ${src}) of=${src}.bak conv=sync,noerror
	cmp ${src} ${src}.bak
	if [ $? -ne 0 ]; then
		echo "Test failed!"
		exit 1
	fi

	rm -f ${test_dir}/$(basename ${src})
done

umount ${test_dir}
echo "Test OK!"
