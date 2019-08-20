#!/usr/bin/env bash

set -x

sudo apt-get install gcc-arm-none-eabi
sudo apt-get install libnewlib-arm-none-eabi
sudo apt-get install qemu-system
arm-none-eabi-gcc --version
qemu-system-arm --version