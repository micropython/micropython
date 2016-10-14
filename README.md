# Adafruit MicroPython

This is an open source derivative of [MicroPython](http://www.micropython.org)
for use on educational development boards designed and sold by Adafruit
including the [Arduino Zero](https://www.arduino.cc/en/Main/ArduinoBoardZero), [Adafruit Feather M0 Basic](https://www.adafruit.com/products/2772), [Adafruit Feather HUZZAH](https://www.adafruit.com/products/2821) and
[Adafruit Feather M0 Bluefruit LE](https://www.adafruit.com/products/2995).

As a MicroPython derivative, this implements Python 3.x on microcontrollers such
as the SAMD21 and ESP8266.

## Project Status
This project is in beta and the APIs will change in the near future.

## Documentation

Guides and videos are available through the [Adafruit Learning System](https://learn.adafruit.com/) under the [MicroPython category](https://learn.adafruit.com/category/micropython). An API reference is also available on [Read the Docs](http://adafruit-micropython.readthedocs.io/en/latest/?).

## Contributing
See [CONTRIBUTING.md](https://github.com/adafruit/micropython/blob/master/CONTRIBUTING.md)
for full guidelines but please be aware that by contributing to this project you
are agreeing to the
[Code of Conduct](https://github.com/adafruit/micropython/blob/master/CODE_OF_CONDUCT.md).
Contributors who follow the
[Code of Conduct](https://github.com/adafruit/micropython/blob/master/CODE_OF_CONDUCT.md)
are welcome to submit pull requests and they will be promptly reviewed by
project admins.

## Project Structure
Here is an overview of the top-level directories.

### Core
The core of MicroPython is code shared amongst ports.
- `docs` High level user documentation in Sphinx reStructuredText format.
- `drivers` External device drivers written in Python.
- `examples` A few example Python scripts.
- `extmod` Shared C code used in multiple ports' modules.
- `lib` Shared core C code including externally developed libraries such as FATFS.
- `logo` The MicroPython logo.
- `mpy-cross` A cross compiler that converts Python files to byte code prior to being run in MicroPython. Useful for reducing library size.
- `py` Core Python implementation, including compiler, runtime, and
  core library.
- `shared-bindings` Shared definition of Python modules, their docs and backing C APIs. Ports must implement the C API to support the corresponding module.
- `tests` Test framework and test scripts.
- `tools` Various tools, including the pyboard.py module.

### Ports
Ports include the code unique to a microcontroller line and also variations
based on the board.
- `atmel-samd` Support for SAMD21 based boards such as [Arduino Zero](https://www.arduino.cc/en/Main/ArduinoBoardZero), [Adafruit Feather M0 Basic](https://www.adafruit.com/products/2772),  and
[Adafruit Feather M0 Bluefruit LE](https://www.adafruit.com/products/2995).
- `bare-arm` A bare minimum version of MicroPython for ARM MCUs.
- `cc3200` Support for boards based [CC3200](http://www.ti.com/product/CC3200) from TI such as the [WiPy 1.0](https://www.pycom.io/solutions/py-boards/wipy1/).
- `esp8266` Support for boards based on ESP8266 WiFi modules such as the [Adafruit Feather HUZZAH](https://www.adafruit.com/products/2821).
- `minimal` A minimal MicroPython port. Start with this if you want
  to port MicroPython to another microcontroller.
- `pic16bit` Support for 16-bit PIC microcontrollers.
- `qemu-arm` Support for ARM emulation through [QEMU](https://qemu.org).
- `stmhal` Support for boards based on STM32 microcontrollers including the MicroPython flagship [PyBoard](https://store.micropython.org/store/#/products/PYBv1_1).
- `teensy` Support for the Teensy line of boards such as the [Teensy 3.1](https://www.pjrc.com/teensy/teensy31.html).
- `unix` Support for UNIX.
- `windows` Support for [Windows](https://www.microsoft.com/en-us/windows/).
- `zephyr` Support for [Zephyr](https://www.zephyrproject.org/), a real-time operating system by the Linux Foundation.

This derivative only maintains the `atmel-samd` and `esp8266` ports. The rest are here to maintain compatibility with the [MicroPython](https://github.com/micropython/micropython) parent project.
