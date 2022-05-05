MicroPython port to WCH-CH32V307
================================

This is an experimental port of MicroPython for the CH32V307EVT board using WCH CH32V307 RISC-V microcontroller.

Supported features include:
- REPL (Python prompt) over UART0.
- Garbage collector (gc)
- Builtin modules: gc, array, collections, io, struct, sys.
- GPIO interface with machine.Pin module
- Sleep / Delay function from time module

TODO:
- Peripherals - ADC / DAC / SPI / I2C / I2S / RTC support.
- Ethernet / USBFS / USBHS
- Internal Filesystem using the flash.

Build instructions
------------------

Install the toolchain bundle downloaded from the [Mounriver](http://www.mounriver.com/download) website. Edit Makefile to change path variables TOOLCHAIN_ROOT and OPENOCD_ROOT to the gcc toolchain and openocd bin directory. Rename directories to avoid spaces in path string.

The MicroPython cross-compiler must be built to pre-compile some of the
built-in scripts to bytecode.  This can be done using:
```bash
$ make -C mpy-cross
```

Then, to build MicroPython for the CH32V307, just run:
```bash
$ cd ports/wch
$ make -j`nproc`
```

To flash MicroPython image to your CH32V307, connect USB cable to P9 port and use:
```bash
$ make prog
```

To access the MicroPython REPL running on CH32V307, use and select `WCH-Link`:
```bash
$ miniterm.py - 115200 --raw
```

Examples
--------

```python
MicroPython v1.18-131-g31ba059e9-dirty on 2022-05-05; ch32v307-evt with wch-ch32v307
Type "help()" for more information.
>>>
>>> import machine
>>> machine.info()
DEVID=0x0518
REVID=0x3070
S=72000000
H=72000000
P1=36000000
P2=72000000
qstr:
  n_pool=1
  n_qstr=5
  n_str_data_bytes=20
  n_total_bytes=196
GC:
  3968 total
  2544 : 1424
  1=35 2=18 m=32
```

Connect jumper wires between pads - PA0 <--> LED1 and PE0 <--> KEY.

```python
MicroPython v1.18-131-g31ba059e9-dirty on 2022-05-05; ch32v307-evt with wch-ch32v307
Type "help()" for more information.
>>>
>>> from machine import Pin
>>> import time
>>>
>>> pa0=Pin(Pin.board.PA0, Pin.OUT)
>>>
>>> for i in range(5):
...   pa0.off()
...   time.sleep(1)
...   pa0.on()
...   time.sleep(1)
...   print("Iter: " + str(i))
...                                                 # Press Enter and watch LED blink 5 times
Iter:0
Iter:1
Iter:2
Iter:3
Iter:4
>>>
>>> pe0=Pin(Pin.board.PE0, Pin.IN)
>>> pe0.value()                                     # USER button not pressed
1
>>> pe0.value()                                     # USER button pressed
0
>>> pe0.value()                                     # USER button pressed
0
>>> pe0.value()                                     # USER button not pressed
1
>>> while True:                                     # Pressing USER button lights up LED1
...   pa0.value(pe0.value())
...                                                 # Press Enter

```
Since interrupt character for breaking out of infinite loop is not implemented, you might have to RESET the board to reload the REPL.

Enjoy :)
