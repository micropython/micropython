#!/bin/bash

# Clear build directories

ORIGINAL_DIR=$PWD

source env-variables-micropython.sh

# Clean mpy-cross
cd $MICROPYTHON/mpy-cross
rm -rf build-*

# Clean Unix port
cd $MICROPYTHON/ports/unix
rm -rf build-*

# Clean ESP32 port boards
cd $ORIGINAL_DIR
source env-variables-esp32.sh
cd $MICROPYTHON/ports/esp32
rm -rf build-*

# Clean Raspberry boards
cd $MICROPYTHON/ports/rp2
rm -rf build-*

cd $ORIGINAL_DIR