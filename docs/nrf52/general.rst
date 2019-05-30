.. _nRF52832_general:

General information about the nRF52832 port
========================================

The nRF52832 is a popular Bluetooth enabled System-on-Chip (SoC) by
Espressif Systems.

Multitude of boards
-------------------

There is a multitude of modules and boards from different sources which carry
the nRF52832 chip. MicroPython tries to provide a generic port which would run on
as many boards/modules as possible, but there may be limitations. nRF52832
development boards are taken as reference for the port (for example, testing is
performed on them).  For any board you are using please make sure you have a
datasheet, schematics and other reference materials so you can look up any
board-specific functions.

To make a generic nRF52832 port and support as many boards as possible the
following design and implementation decision were made:

* GPIO pin numbering is based on nRF52832 chip numbering.  Please have the manual/pin
  diagram of your board at hand to find correspondence between your board pins and
  actual nRF52832 pins.
* All pins are supported by MicroPython but not all are usable on any given board.
  and a board may only expose a certain selection of pins.


Technical specifications and SoC datasheets
-------------------------------------------

The datasheets and other reference material for nRF52832 chip are available
from the vendor site: https://www.nordicsemi.com/ .
They are the primary reference for the chip technical specifications, capabilities,
operating modes, internal functioning, etc.

For your convenience, some of technical specifications are provided below:

* Architecture: ARM Cortex-M4F 32-bit
* CPU frequency: up to 64MHz
* Flash available: 512KB
* RAM: 64KB
* GPIO: 32 (GPIOs are multiplexed with other functions, including
  external SWD, UART, etc.)
* UART: 1 RX/TX UART (hardware handshaking)
* SPI: 3 SPI interfaces,support soft SPI and Hard SPI.
* I2C: 2 I2C
* I2S: 1
* PWM: 3 PWM interfaces.
* Timer: 5 Timers.
* RTCounter: 3 Real time  clock Counter.
* temp: temperature register.
* ADC: 12-bit SAADC up to 8 channels
* Programming: using bootloader from UART -
  and always-available BootROM bootloader

For more information see the nRF52832 datasheet: https://www.nordicsemi.com/Products/Low-power-short-range-wireless/nRF52832

