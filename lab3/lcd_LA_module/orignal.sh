#!/bin/sh

module_lcd0=lcd0_LA

/sbin/insmod ./$module_lcd0.ko || {                                     # Insert the lcd_LA module
    echo "Error: Failed to insert $module_lcd0 module"
    exit 1
}
rm -f /dev/$module_lcd0                                                 # Remove the previous node for lcd_LA
major_lcd0=$(grep $module_lcd0 /proc/devices | awk '{print $1}')        # Get the major number for lcd_LA module
mknod /dev/$module_lcd0 c $major_lcd0 0                                 # Create a new node for lcd_LA module
chmod 666 /dev/$module_lcd0                                             # Change the permissions of the node to allow read-write access
echo "=> Device /dev/$module_lcd0 created with major=$major_lcd0"       # Display a message indicating successful creation of the node

module_lcd1=lcd1_LA

/sbin/insmod ./$module_lcd1.ko || {                                     # Insert the lcd_LA module
    echo "Error: Failed to insert $module_lcd1 module"
    exit 1
}
rm -f /dev/$module_lcd1                                                 # Remove the previous node for lcd_LA
major_lcd1=$(grep $module_lcd1 /proc/devices | awk '{print $1}')        # Get the major number for lcd_LA module
mknod /dev/$module_lcd1 c $major_lcd1 0                                 # Create a new node for lcd_LA module
chmod 666 /dev/$module_lcd1                                             # Change the permissions of the node to allow read-write access
echo "=> Device /dev/$module_lcd1 created with major=$major_lcd1"       # Display a message indicating successful creation of the node
