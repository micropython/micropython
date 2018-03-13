# MicroPython branch: stm32-ethernet-F7DISC

This branch contains STM32 ethernet module for LwIP.

Current supported board is STM32F746G-Discovery and NUCLEO_F767ZI.
(I don't test NUCLEO_F767ZI and WIZnet module, because I don't have.)

## How to build

```
git clone -b stm32-ethernet-F7DISC https://github.com/forester3/micropython.git
cd micropython
git submodule update --init
make -C mpy-cross
make -C ports/stm32 MICROPY_PY_ETH=RMII BOARD=STM32F7DISC
```

## How to build original WIZnet5K MACRAW mode support

```
make -C ports/stm32 MICROPY_PY_ETH=WIZNET5K MICROPY_PY_WIZNET5K=5500 BOARD=YOUR_BOARD
```
