# !/bin/bash

MICROPYTHON_ROOT=$(git rev-parse --show-toplevel)

if [ "$1" = "esp32" ]
then
    cd "$MICROPYTHON_ROOT/ports/esp32"
    make -j$(nproc)
else
    cd "$MICROPYTHON_ROOT/ports/unix"
    make USER_C_MODULES=../../mhm_custom_extensions/ -j$(nproc)
fi