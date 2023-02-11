#!/bin/sh

module_led0=led0_LA
module_bp=bp_LA

/sbin/insmod ./$(module_led0).ko  || exit 1
rm -f /dev/$(module_led0)
major=$(awk $(module_led0) /proc/devices)
mknod /dev/$(module_led0) c $(major) 0
chmod 666 /dev/$(module_led0)
echo "=> Device /dev/$(module_led0) created with major=$(major)"

/sbin/insmod ./$(module_bp).ko  || exit 1
rm -f /dev/$(module_bp)
major=$(awk $(module_bp) /proc/devices)
mknod /dev/$(module_bp) c $(major) 0
chmod 666 /dev/$(module_bp)
echo "=> Device /dev/$(module_bp) created with major=$(major)"

