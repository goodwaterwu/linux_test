#!/bin/sh
#
# Test GPIOs.
#
# author: Wu, Jheng-Jhong (goodwater.wu@gmail.com)
# version: 20211229

### GPIO information ###

test_set_direction()
{
	echo "$2" > /sys/class/gpio/gpio$1/direction
	if [ $? -ne 0 ]; then
		echo "change direction into $2 failed"
		echo $1 >> failed_list
		return 1
	fi
}

test_set_value()
{
	echo "$2" > /sys/class/gpio/gpio$1/value
	if [ $? -ne 0 ]; then
		echo "set value to $2 failed"
		echo $1 >> failed_list
		return 1
	fi
}

test_gpio()
{
	if [ "$2" == "in" ]; then
		test_set_direction $1 "out" || return 1
		if [ $3 -eq 0 ]; then
			test_set_value $1 1
		else
			test_set_value $1 0
		fi
	else
		if [ $3 -eq 0 ]; then
                        test_set_value $1 1
                else
                        test_set_value $1 0
                fi
		if [ $? -eq 0 ]; then
			test_set_direction $1 "in" || return 1
		fi
	fi

	test_set_direction $1 $2
	if [ $(cat /sys/class/gpio/gpio$1/direction) == "out" ]; then
		test_set_value $1 $3
	fi
}

show_info()
{
	echo "----- gpio$1 -----"
	direction=$(cat /sys/class/gpio/gpio$1/direction)
	value=$(cat /sys/class/gpio/gpio$1/value)
	edge=$(cat /sys/class/gpio/gpio$1/edge)
	echo "active low: $(cat /sys/class/gpio/gpio$1/active_low)"
	echo "direction: ${direction}"
	echo "value: $(cat /sys/class/gpio/gpio$1/value)"
	echo "edge: ${edge}"

	if [ "${edge}" != none ]; then
		echo "skip testing gpio$1"
		return 0
	fi

	test_gpio $1 ${direction} ${value}
}

### Main program ###

rm -f gpiochips 2>/dev/null
rm -f exported_gpios 2>/dev/null
rm -f unexported_gpios 2>/dev/null
rm -f failed_list 2>/dev/null

ls /sys/class/gpio/ | grep 'gpiochip' >> gpiochips
gpios=0

for chip in $(cat gpiochips); do
	ngpio=$(cat /sys/class/gpio/$chip/ngpio)
	gpios=$((gpios + ngpio))
done

for i in $(seq 0 $((gpios - 1))); do
	if [ ! -d /sys/class/gpio/gpio$i ]; then
		echo $i > /sys/class/gpio/export 2>/dev/null || echo -n "$i " >> unexported_gpios
	fi
done

echo "" >> unexported_gpios

for i in $(seq 0 $((gpios - 1))); do
	if [ -d /sys/class/gpio/gpio$i ]; then
		echo -n "$i " >> exported_gpios
	fi
done

echo "" >> exported_gpios

for g in $(cat exported_gpios); do
	show_info $g
done

echo "Number of GPIO chips: $(cat gpiochips | wc -w)"
echo "Number of GPIOs: ${gpios}"

echo -n "Unexported GPIOs: "
if [ -f unexported_gpios ]; then
	cat unexported_gpios
else
	echo "None"
fi

echo -n "Test failed: "
if [ -f failed_list ]; then
	cat failed_list
else
	echo "None"
fi

rm -f gpiochips 2>/dev/null
rm -f exported_gpios 2>/dev/null
rm -f unexported_gpios 2>/dev/null
rm -f failed_list 2>/dev/null
