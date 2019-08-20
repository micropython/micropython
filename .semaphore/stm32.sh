#!/usr/bin/env bash

set -x

git submodule update --init lib/lwip lib/mbedtls lib/stm32lib
make ${MAKEOPTS} -C mpy-cross
make ${MAKEOPTS} -C ports/stm32
make ${MAKEOPTS} -C ports/stm32 BOARD=PYBV11 MICROPY_PY_WIZNET5K=5200 MICROPY_PY_CC3K=1
make ${MAKEOPTS} -C ports/stm32 BOARD=PYBD_SF2
make ${MAKEOPTS} -C ports/stm32 BOARD=STM32F769DISC CFLAGS_EXTRA='-DMICROPY_PY_THREAD=1'
make ${MAKEOPTS} -C ports/stm32 BOARD=STM32L476DISC
make ${MAKEOPTS} -C ports/stm32/mboot BOARD=PYBD_SF6
