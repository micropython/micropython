.. _samd_general:

General information about the SAMD port
=======================================

The SAMD21/SAMD51 MCU family is a high performance family of devices made by MicroChip.
The SAMD21 devices are based on an ARM M0+ core, the SAMD51 device on ARM Cortex M4 core.
They provide many on-chip I/O units for building small to medium sized devices.

Multitude of boards
-------------------

There is a multitude of modules and boards from different sources which carry
an SAMD21/SAMD51 chip.  MicroPython aims to provide a generic port which runs on
as many boards/modules as possible, but there may be limitations.  The
Adafruit ItsyBitsy M0 Express, Adafruit Feather M4 Express and the Adafruit ItsyBitsy M4 Express
development boards are taken as reference for the port (for example, testing is performed on them).
For any board you are using please make sure you have a data sheet, schematics
and other reference materials so you can look up any board-specific functions.

The following boards are at the moment supported by the port:

- ADAFRUIT FEATHER M0 EXPRESS
- ADAFRUIT FEATHER M4 EXPRESS
- ADAFRUIT ITSYBITSY M0 EXPRESS
- ADAFRUIT ITSYBITSY M4 EXPRESS
- ADAFRUIT TRINKET M0
- MINISAM M4
- SAMD21 XPLAINED PRO
- SEEED WIO TERMINAL
- SEEED XIAO

To make a generic SAMD port and support as many boards as possible the
following design and implementation decision were made:

* GPIO pin numbering is based on the board numbering.
  Please have the manual/pin diagram of your board at hand
  to find correspondence between your board pins and actual SAMD21/SAMD51 pins.
  For the boards listed above, the relation between the board pin number and
  the GPIO number can be found at :ref:`samd_pinout`.
* The pins that can be used by MicroPython are limited to those listed
  in the board definition files.

Technical specifications and SoC data sheets
--------------------------------------------

The data sheets and other reference material for SAMD21/SAMD51 chip are available
from the vendor site: https://www.microchip.com/en-us/products/microcontrollers-and-microprocessors/32-bit-mcus/sam-32-bit-mcus
They are the primary reference for the chip technical specifications, capabilities,
operating modes, internal functioning, etc.

For your convenience, a few technical specifications are provided below:

SAMD21:

* Architecture: ARM Cortex M0+
* CPU frequency: up to 48MHz
* Total RAM available: up to 32 kB (see table)
* Internal FlashROM: up to 256 kB
  Some boards provide additional external SPI flash.
* GPIO: up to 52 (GPIOs are multiplexed with other functions, including
  external FlashROM, UART, etc.)
* UART: up to 6 serial devices, which can used for UART, SPI or I2C.
* I2S: 1 I2S interfaces
* ADC: One 12-bit SAR ADC converter with 16 channels.
* Programming: using BootROM bootloader from USB.

SAMD51:

* Architecture: ARM Cortex M4
* CPU frequency: up to 120MHz
* Total RAM available: up to 256 kB
* Internal FlashROM: up to 1 MB
  Some boards provide additional external SPI flash.
* GPIO: up to 99 (GPIOs are multiplexed with other functions, including
  external FlashROM, UART, etc.)
* UART: up to 8 serial devices, which can used for UART, SPI or I2C.
* I2S: 1 I2S interfaces
* ADC: Dual 12-bit SAR ADC converter with 16 channels.
* Programming: using BootROM bootloader from USB.


For more information see the excellent SAMD21/SAMD51 data sheets or reference manuals.

At the moment, the SAM21 port of MicroPython requires 256kB flash, of which 64kB is used
for a small file system. The SAM51 port requires 512 kB of flash, of which all flash beyond
384kB is used for a file system. Some boards have additional flash memory, which can be used
as additional file space.
