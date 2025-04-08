#!/usr/bin/env bash

cd `dirname $(readlink -f "$0")`

idf_path=`echo ~/.espressif/python_env/idf5.2*_env/bin`
export PATH=$idf_path:$PATH

cd ~/projects/esp-idf.v5.2.2
source export.sh

cd -

if [ ! "$1" ]; then
	rm -rf build-ESP32_GENERIC_C3
fi

make -j4 BOARD=ESP32_GENERIC_C3
