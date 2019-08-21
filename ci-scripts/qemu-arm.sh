#!/usr/bin/env bash

set -x

make ${MAKEOPTS} -C mpy-cross
make ${MAKEOPTS} -C ports/qemu-arm -f Makefile.test test
