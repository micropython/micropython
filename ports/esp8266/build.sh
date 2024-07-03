#!/bin/bash

export PATH=~/.espressif/python_env/idf5.0_py3.11_env/bin:$PATH

if [ ! "$1" ]; then
	make clean
fi

make -j BOARD=ESP8266_GENERIC

