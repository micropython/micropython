#!/usr/bin/env bash

cd `dirname $(readlink -f "$0")`

export PATH=~/.espressif/python_env/idf5*_env/bin:$PATH

cd ~/projects/esp-idf
source export.sh

cd -

if [ ! "$1" ]; then
	rm -rf build-ESP32_GENERIC_C3
fi

make BOARD=ESP32_GENERIC_C3 -j4
