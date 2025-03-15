#!/bin/bash

# Deploy firmware ESP32 boards

source env-variables-micropython.sh
source env-variables-esp32.sh

source menu-esp32.sh
if [ -z "$BOARD" ]; then
    exit 1
fi

cd $MICROPYTHON/ports/esp32
make deploy BOARD=$BOARD BOARD_VARIANT=$BOARD_VARIANT
