#!/bin/bash

export PATH=`echo ~/.espressif/python_env/idf5.*env/bin`:$PATH

if [ ! "$1" ]; then
	make clean
fi

make -j BOARD=ESP8266_GENERIC

