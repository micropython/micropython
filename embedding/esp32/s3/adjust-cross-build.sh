#!/bin/bash

cp ./xtensa-cross.cmake.tpl ./xtensa-cross.cmake
sed -i "s#\[HOMEDIR\]#${HOME}#g" ./xtensa-cross.cmake
