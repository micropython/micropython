# SAMD21x18

This port is intended to be a full featured MicroPython port for SAMD21x18 based
development boards including the Arduino Zero, Adafruit Feather M0 and Adafruit
M0 BLE.

## Building for a SAMD21x18 MCU

The Makefile has the ability to build for a SAMD21x18, and by default
includes some start-up code and also enables a UART for communication.  To build:

    $ make CROSS=1

If you previously built the Linux version, you will need to first run
`make clean` to get rid of incompatible object files.

Deploying coming soon!
