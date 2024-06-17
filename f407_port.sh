#!/bin/bash


# chmod +x py_env.sh

make -C mpy-cross


# 构建 STM32 目标
cd ports/stm32

# 设置默认的板名称
BOARD=Multi_Port_Debug_F407ZE 


make BOARD=$BOARD  submodules

make BOARD=$BOARD  MICROPY_PY_NETWORK_WIZNET5K=5500