#!/bin/sh

# Define the name of the lcd
module_lcd=lcd0_LA


/sbin/insmod ./$module_lcd.ko || {                                     # Insert the lcd_LA module
    echo "Error: Failed to insert $module_lcd module"
    exit 1
}
rm -f /dev/$module_lcd                                                 # Remove the previous node for lcd_LA
major_led0=$(grep $module_lcd /proc/devices | awk '{print $1}')        # Get the major number for lcd_LA module
mknod /dev/$module_lcd c $major_led0 0                                 # Create a new node for lcd_LA module
chmod 666 /dev/$module_lcd                                             # Change the permissions of the node to allow read-write access
echo "=> Device /dev/$module_lcd created with major=$major_led0"       # Display a message indicating successful creation of the node
