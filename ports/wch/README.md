MicroPython port to WCH-CH32V307
================================

This is an experimental port of MicroPython for the CH32V307EVT board using WCH CH32V307 RISC-V microcontroller.

Supported features include:
- REPL (Python prompt) over UART0.
- Garbage collector (gc)
- Builtin modules: gc, array, collections, io, struct, sys.
  
TODO:
- GPIO / SPI / I2C / I2S support.
- Ethernet / Networking stack
- Internal filesystem using the flash.

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

