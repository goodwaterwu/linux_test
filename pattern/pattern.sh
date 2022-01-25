#!/bin/sh
#
# Generate a pattern file.
#
# author: Victor Wu (goodwater.wu@gmail.com)
# version: 20220124

### Functions ###

show_help()
{
	echo -e "\nGenerate a pattern file"
	echo -e "\nUsage: $(basename $0) [-f file] [-l lower] [-u upper] [-s size] [-h]"
	echo -e "\t-f\tfile path (Cannot overwrite an exist file)"
	echo -e "\t-l\tlowest hexadecimal number (0-255)"
	echo -e "\t-u\tlargest hexadecimal number (0-255)"
	echo -e "\t-s\tfile size in bytes"
	echo -e "\t-h\tshow this help\n"
}

### Main program ###

pattern=hex_pattern
lower=0
upper=255
file_size=1024
ch=0

options="f:l:s:u:h"
while getopts ${options} opt
do
	case ${opt} in
	f)
		pattern=${OPTARG};;
	l)
		lower=${OPTARG};;
	u)
		upper=${OPTARG};;
	s)
		file_size=${OPTARG};;
	*)
		show_help $0
		exit 0;;
	esac
done

if [ -f ${pattern} ] ||
		[ ${lower} -gt ${upper} ] || 
		[ ${lower} -lt 0 -o ${lower} -gt 255 ] || 
		[ ${upper} -lt 0 -o ${upper} -gt 255 ] ||
		[ ${file_size} -lt 1 ]; then
	show_help $0
	exit 1
fi

ch=${lower}
for i in $(seq 0 $((file_size - 1))); do
	if [ $((ch % (upper + 1))) -eq 0 ]; then
		ch=${lower}
	fi

	echo -en "\x$(printf %02x ${ch})" >> ${pattern}
	ch=$((ch + 1))
done
