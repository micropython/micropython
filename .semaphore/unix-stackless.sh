#!/usr/bin/env bash

set -x

git submodule update --init lib/axtls lib/berkeley-db-1.xx lib/libffi
make ${MAKEOPTS} -C mpy-cross CC=clang
make ${MAKEOPTS} -C ports/unix CC=clang CFLAGS_EXTRA="-DMICROPY_STACKLESS=1 -DMICROPY_STACKLESS_STRICT=1"
make ${MAKEOPTS} -C ports/unix CC=clang test
