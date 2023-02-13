#!/bin/sh

# Define the name of the led0 and bp module
module_led0=led0_LA
module_bp=bp_LA


/sbin/insmod ./$module_led0.ko || {                                     # Insert the led0 module
    echo "Error: Failed to insert $module_led0 module"
    exit 1
}
rm -f /dev/$module_led0                                                 # Remove the previous node for led0
major_led0=$(grep $module_led0 /proc/devices | awk '{print $1}')        # Get the major number for led0 module
mknod /dev/$module_led0 c $major_led0 0                                 # Create a new node for led0 module
chmod 666 /dev/$module_led0                                             # Change the permissions of the node to allow read-write access
echo "=> Device /dev/$module_led0 created with major=$major_led0"       # Display a message indicating successful creation of the node


/sbin/insmod ./$module_bp.ko || {                                       # Insert the bp module
    echo "Error: Failed to insert $module_bp module"
    exit 1
}
rm -f /dev/$module_bp                                                   # Remove the previous node for bp
major_bp=$(grep $module_bp /proc/devices | awk '{print $1}')            # Get the major number for bp module
mknod /dev/$module_bp c $major_bp 0                                     # Create a new node for bp module
chmod 666 /dev/$module_bp                                               # Change the permissions of the node to allow read-write access
echo "=> Device /dev/$module_bp created with major=$major_bp"           # Display a message indicating successful creation of the node

