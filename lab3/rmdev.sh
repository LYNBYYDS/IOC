#!/bin/sh

module_led0=led0_LA
module_led1=led1_LA
module_bp=bp_LA

/sbin/rmmod $module_led0 || exit 1
rm -f /dev/$module_led0
echo "=> Device /dev/$module_led0 removed"

/sbin/rmmod $module_led1 || exit 1
rm -f /dev/$module_led1
echo "=> Device /dev/$module_led1 removed"

/sbin/rmmod $module_bp || exit 1
rm -f /dev/$module_bp
echo "=> Device /dev/$module_bp removed"

