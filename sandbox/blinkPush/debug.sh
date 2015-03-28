#!/bin/bash

make
arm-none-eabi-gdb blinkPush.elf -x startGDB.gdb
./clean.sh
