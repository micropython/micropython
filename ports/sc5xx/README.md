MicroPython port to SC5XX processors
====================================

This directory contains the port of MicroPython to ADI's ADSP-SC5xx series
processors. Parts of the code utilize ADI's Baremetal SDK, which is licensed
under the clear BSD license. This port support the following boards:
- ADI SHARC Audio Module

Build instructions
------------------

ADI's CrossCore Embedded Studio (CCES) is required to build this specific port
of MicroPython. Both Windows version and Linux version of CCES are supported.

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
