#!/usr/bin/env bash

set -x

git submodule update --init lib/axtls lib/berkeley-db-1.xx lib/libffi
make ${MAKEOPTS} -C mpy-cross
make ${MAKEOPTS} -C ports/unix deplibs
make ${MAKEOPTS} -C ports/unix nanbox
(cd tests && MICROPY_CPYTHON3=python3 MICROPY_MICROPYTHON=../ports/unix/micropython_nanbox ./run-tests)
