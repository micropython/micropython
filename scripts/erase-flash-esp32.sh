#!/bin/bash

# Erase the flash memory of the ESP32 board

source env-variables-micropython.sh
source env-variables-esp32.sh

source menu-esp32.sh
if [ -z "$BOARD" ]; then
    exit 1
fi

cd $MICROPYTHON/ports/esp32
make erase BOARD=$BOARD