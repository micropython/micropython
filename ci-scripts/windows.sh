#!/usr/bin/env sh

set -x

make ${MAKEOPTS} -C mpy-cross
make ${MAKEOPTS} -C ports/windows CROSS_COMPILE=i686-w64-mingw32-
