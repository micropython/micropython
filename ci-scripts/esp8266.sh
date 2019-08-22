#!/usr/bin/env sh

set -x

git submodule update --init lib/axtls lib/berkeley-db-1.xx
make ${MAKEOPTS} -C mpy-cross
make ${MAKEOPTS} -C ports/esp8266
