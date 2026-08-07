#!/bin/bash

cp ./riscv32-cross.cmake.tpl ./riscv32-cross.cmake
sed -i "s#\[HOMEDIR\]#${HOME}#g" ./riscv32-cross.cmake
