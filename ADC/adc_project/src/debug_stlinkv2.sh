#!/bin/bash
#
# Start OpenOCD GDB server + connect to it
#
# written by Greg Knoll 2013


#[ -e required_version ] || { echo "You need to be in the project directory"; exit 1;}

Script1="startscript_stlinkv2.gdb"
if [ ! -e $Script1 ]; then
  cat <<END_OF_SCRIPT >$Script1

target extended-remote :4242
load

END_OF_SCRIPT
  echo "missing gdb start-script '`pwd`/$Script1'"
fi

killall -9 --wait st-util
st-util >st-util.log 2>&1 &
sleep 1
arm-none-eabi-gdb "$1" -x $Script1
killall -9 --wait st-util

#break Assert_Halt_EC
