#!/bin/bash

make && arm-none-eabi-objcopy -O binary blinkPush.elf blinkPush.bin && st-flash write blinkPush.bin 0x8000000
./clean.sh
