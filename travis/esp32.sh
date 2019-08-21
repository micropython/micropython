#!/usr/bin/env bash

set -x

git submodule update --init lib/berkeley-db-1.xx
make ${MAKEOPTS} -C mpy-cross
make ${MAKEOPTS} -C ports/esp32 ESPIDF=$HOME/esp-idf
