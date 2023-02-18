#!/bin/sh

module_lcd=lcd_LA

/sbin/rmmod $module_lcd || exit 1
rm -f /dev/$module_lcd
echo "=> Device /dev/$module_lcd removed"

module_lcd0=lcd0_LA

/sbin/rmmod $module_lcd0 || exit 1
rm -f /dev/$module_lcd0
echo "=> Device /dev/$module_lcd0 removed"

module_lcd1=lcd1_LA

/sbin/rmmod $module_lcd1 || exit 1
rm -f /dev/$module_lcd1
echo "=> Device /dev/$module_lcd1 removed"

module_lcd2=lcd2_LA

/sbin/rmmod $module_lcd2 || exit 1
rm -f /dev/$module_lcd2
echo "=> Device /dev/$module_lcd2 removed"

module_lcd3=lcd3_LA

/sbin/rmmod $module_lcd3 || exit 1
rm -f /dev/$module_lcd3
echo "=> Device /dev/$module_lcd3 removed"