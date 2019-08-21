#!/usr/bin/env bash

set -x

make ${MAKEOPTS} -C ports/bare-arm
make ${MAKEOPTS} -C ports/minimal CROSS=1 build/firmware.bin
ls -l ports/minimal/build/firmware.bin
tools/check_code_size.sh
mkdir -p ${HOME}/persist
