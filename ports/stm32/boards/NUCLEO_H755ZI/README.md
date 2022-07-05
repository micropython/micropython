**Intro.**

The config based on NUCLEO_H743ZI2 board config.

**Specific.**

The config requires to use a prefix CFLAGS=-DCORE_CMx when build the firmware for M4/M7 core respectively.
```
CFLAGS=-DCORE_CM4 make BOARD=NUCLEO_H755ZI
```

The macro definition required by STM HAL.


**Features.**

Both cores work simultaneously.

M7 has serial console on UART3 (micro-USB power connector).

M4 has serial console on UART6 (C6,C7 -> Rx/Tx on CN7, Gnd on CN10).

CTRL-D - soft reset only the core, not neighbour.

Advice: separate the hardware resources between the cores for its doesn't interfere to each other.

As example: LED3 (red) is allocated for M4. M7 core cann't manage the led, but both cores can turn on/off LED1/2.

Edit NUCLEO_H755ZI/mpconfigboard.h to allocate hardware between the cores.

**Bugs.**

Some functions lead to hard reset the board (ex. pyb.info(), network.LAN() on M4)
