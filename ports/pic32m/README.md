MicroPython port to the PIC32M
==============================

This is an experimental port of MicroPython to the PIC32M series microcontroller
from Microchip. The experiment is done with PIC32MX170F256B in 28-pin DIP 
package for easy hand soldering. 

Supported features include:
- REPL (Python prompt) over UART1.
- The pyb module with 1 Led and 1 Switch.

PIC32MX170F256B Hardware configuration
--------------------------------------

- VDD(3.3V): Pin13, Pin28
- VSS(0V): Pin8, Pin19, Pin27
- VCAP: Pin20, connecting 47uF capacitor to VSS
- MCLR: Pin1, connecting a 10kOhm pull-up resistor to VDD
- PGED1: Pin4, RB0
- PGEC1: Pin5, RB1
- TX1: Pin11, RB4
- RX1: Pin15, RB6, connecting a 10kOhm pull-up resistor to VDD
- Output(Led): Pin25, RB14
- Input(Switch): Pin26, RB15

Firmware toolchain
------------------

- XC32 v2.30
- (Optional) MPLAB X IDE v5.10 or above
- (Optional) MCC v3.85.1 or above (it is a MPLAB plugin for easy generating configuration files)

The firmware is compiled and linked in Windows MinGW environment. For Linux, 
some simple modifications in Makefile is needed.

Building the firmware
---------------------

In Windows MinGW:
```bash
$ make PYTHON=python
```

In Linux(modifications needed in Makefile):
```bash
$ make
```

And find firmware.hex in build folder.

Getting a Python prompt
-----------------------

You can get a prompt via the serial port, via UART1. The baudrate for the REPL 
is 9600 and:

In Windows, Tera Term VT is good for the job.

In Linux, try
```bash
$ picocom -b 9600 /dev/ttyUSB0
```

Some brief test result
----------------------

```python
MicroPython v1.11 on 2019-11-07; EXPERIMENT with PIC32MX170F256B
>>> a,b=1,2
>>> c=a+b
>>> c
3
>>> 256*256
65536
>>> 100//3
33
>>> import pyb
>>> led=pyb.LED(1)
>>> led.off() # The LED turned off.
>>> led.on() # The LED turned on.
>>> sw=pyb.Switch(1)
>>> sw.value()
True
>>> sw.value() # The switch is pressed.
False
>>> list(5 * x + y for x in range(10) for y in [4, 2, 1])
[4, 2, 1, 9, 7, 6, 14, 12, 11, 19, 17, 16, 24, 22, 21, 29, 27, 26, 34, 32, 31, 39, 37, 36, 44, 42, 41, 49, 47, 46]
>>> import gc
>>> gc.mem_free()
13392
>>> gc.collect()
>>> gc.mem_free()
15392
```
