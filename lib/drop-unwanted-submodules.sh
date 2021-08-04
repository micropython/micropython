#!/bin/sh

UNWANTED_SUBMODULES="alif_ensemble-cmsis-dfp alif-security-toolkit \
    arduino-lib asf4 axtls berkeley-db-1.xx btstack cyw43-driver \
    fsp libffi libhydrogen libmetal lwip mbedtls micropython-lib \
    mynewt-nimble nrfx nxp_driver open-amp pico-sdk protobuf-c \
    tinyusb wiznet5k"

cd $(dirname $0)

for SUBMODULE in $UNWANTED_SUBMODULES; do
    git submodule deinit -f -- ./$SUBMODULE
    rm -rf ../.git/modules/lib/$SUBMODULE
    git rm -f ./$SUBMODULE
done
