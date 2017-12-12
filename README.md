# MicroPython branch: stm32-ethernet

This branch contains STM32 ethernet module for LwIP.

Current supported board is NUCLEO_F767ZI.

## How to build

```
git clone -b stm32-ethernet https://github.com/boochow/micropython.git
cd micropython
git submodule update --init
make -C mpy-cross
make -C ports/stm32 MICROPY_PY_ETH=RMII BOARD=NUCLEO_F767ZI
```

## How to build original WIZnet5K MACRAW mode support

```
make -C ports/stm32 MICROPY_PY_ETH=WIZNET5K MICROPY_PY_WIZNET5K=5500 BOARD=YOUR_BOARD
```