**INTRO**

The config referenses on NUCLEO_H743ZI2 board config.

**BUILD**

As stm32h755 has two cores (M4/M7) there is specific to build firmware for them.
It's needed to use a prefix CFLAGS=-DCORE_CMx , where "x" is "4" or "7". For M7 core use:

```
CFLAGS=-DCORE_CM7 make BOARD=NUCLEO_H755ZI
```

for M4 core use:

```
CFLAGS=-DCORE_CM4 make BOARD=NUCLEO_H755ZI
```

Don't forget to use:

```
make BOARD=NUCLEO_H755ZI clean
```

**FEATURES**

Both cores work simultaneusly and independently. Advice: separate the board hardware resources between the cores (edit mpconfigboard.h).
Ethernet bond (by default) with M4 core, but doesn't work. Rebind it with M7 to get it's working.
LED3(red) bond with M4 and cannot be managed with M7. But LED1/2 can be switched on/off by both cores.

Core M7 uses UART3 (on micro-USB power connector) as console.
Core M4 uses UART6 (pins PC6/PC7 dubbed on CN7 connector, and GND on CN10).

CTRL-D - soft reset only the core, not affect on neighbour.

SERVO enabled on M4 (not tested yet). Must work on PA0-PA3.

**BUGS**

Hard reset the board occured after some functions on core M4, for instance:

pyb.info(), network.LAN() and some other.

