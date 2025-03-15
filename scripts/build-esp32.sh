#!/bin/bash

# Build MicroPython-LVGL firmware for ESP32 boards

source env-variables-micropython.sh
source env-variables-esp32.sh

source menu-esp32.sh
if [ -z "$BOARD" ]; then
    exit 1
fi

cd $MICROPYTHON
make -C mpy-cross

cd $MICROPYTHON/ports/esp32
make BOARD=$BOARD BOARD_VARIANT=$BOARD_VARIANT
