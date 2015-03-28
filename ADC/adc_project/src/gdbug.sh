#!/bin/bash
#
./clean.sh
make
./debug_stlinkv2.sh adc_demo.elf
