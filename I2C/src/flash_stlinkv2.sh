#!/bin/bash
#
# Flash script for STLink V2
#
# Authors: Gregor Rebel (2013)
#
#

Binary="$1"
if [ "$Binary" == "" ]; then
  Binary="main.bin"
fi
if [ ! -e "$Binary" ]; then
  echo "$0 BINARY"
  echo "  ERROR: Cannot find binary $Binary!"
  exit 5
fi

# ensure that no other openocd is currently connected
pkill -9 st-flash && {
    echo "$0 - st-flash was running: killing it"
    sleep 0.2 # short delay, for SIGKILL signal to be processed
}

# use the st-flash function to flash over STLink
st-flash write $Binary 0x8000000 || Error="1"

if [ "$Error" == "" ]; then
  echo "Successfully flashed"
else
  exit 10
fi

