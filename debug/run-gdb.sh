#!/bin/sh

arm-none-eabi-gdb ../kernel.elf -x gdbinit
killall qemu-system-arm

reset
