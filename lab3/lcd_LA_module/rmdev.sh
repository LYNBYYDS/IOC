#!/bin/sh

module_lcd=lcd0_LA

/sbin/rmmod $module_lcd || exit 1
rm -f /dev/$module_lcd
echo "=> Device /dev/$module_lcd removed"
