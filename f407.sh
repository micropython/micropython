#!/bin/bash




make -C mpy-cross


# 构建 STM32 目标
cd ports/stm32

# 设置默认的板名称
BOARD=NUCLEO_F401RE 


make BOARD=$BOARD  submodules

make BOARD=$BOARD  MICROPY_PY_NETWORK_WIZNET5K= W5500