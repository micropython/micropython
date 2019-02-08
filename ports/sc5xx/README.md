MicroPython port to SC5XX processors
====================================

This directory contains the port of MicroPython to ADI's ADSP-SC5xx series
processors. Parts of the code utilize ADI's Baremetal SDK, which is licensed
under the clear BSD license. This port support the following boards:
- ADI SHARC Audio Module

Supported Features
------------------

* I2C (machine.I2C)
* Pins (machine.Pin)
* SDcard (mounted on boot)
* SPI (machine.SPI)
* RTC (access through utime module)

Build instructions
------------------

ADI's CrossCore Embedded Studio (CCES) is required to build this specific port
of MicroPython. Both Windows version and Linux version of CCES are supported. (Note: Linux version could only generate ELF file, but not the LDR file. ELF file can only be loaded onto the board using ICE-1000/2000 and running in the RAM, while the LDR file can be flashed into the SPI Flash. This is a limitation of the CCES Linux version.)

To build the project: Open this directory as an project in the CCES and click
build.

Accessing the Python prompt
---------------------------

You can access the Python prompt via the UART0. On the SAM board, you will need
to use the bundled FTDI cable, on the EZ-BOARD, you can use the on-board USB-to-
serial connection. The baudrate for the REPL is 115200, you can use a command
like this to access the REPL:
```
$ picocom -b 115200 /dev/ttyACM0
```

Using SHARC+ DSP
----------------

The MicroPython only runs on the ARM core, and the MicroPython cannot emit DSP code to be run on the DSP. However it is possible to load precompiled DSP application in the MicroPython environment. There is one builtin module called ```sharc```, which allows booting up SHARC+ DSP during runtime. 

Example:
```
f = open('blinking_led.ldr', 'rb')
stream = f.read()
import sharc
sharc.boot(stream)
```
