#!/bin/sh

# Define the name of the led0 and bp module
led0_module=led0_LA
bp_module=bp_LA

# Function to load a module and create a device node for it
load_module() {
  local module=$1
  local node="/dev/$module"
  local major

  /sbin/insmod ./$module.ko || {
    echo "Error: Failed to insert $module module"
    exit 1
  }

  rm -f $node
  major=$(grep $module /proc/devices | awk '{print $1}')
  mknod $node c $major 0
  chmod 666 $node
  echo "=> Device $node created with major=$major"
}

# Load the led0 module and create a device node for it
load_module $led0_module

# Load the bp module and create a device node for it
load_module $bp_module

