#!/usr/bin/env bash

set -x

git submodule update --init lib/asf4 lib/tinyusb
make ${MAKEOPTS} -C ports/samd
