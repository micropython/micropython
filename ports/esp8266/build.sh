#!/bin/bash

idf_path=`echo ~/.espressif/python_env/idf5.2*_env/bin`
export PATH=$idf_path:$PATH

if [ ! "$1" ]; then
	make clean
fi

make -j4 BOARD=ESP8266_GENERIC

