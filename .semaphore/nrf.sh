#!/usr/bin/env bash

set -x

git submodule update --init lib/nrfx
make ${MAKEOPTS} -C ports/nrf
