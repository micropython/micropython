.. _mimxrt_general:

General information about the MIMXRT port
=========================================

The i.MXRT MCU family is a high performance family of devices made by NXP.
Based on an ARM7 core, they provide many on-chip I/O units for building
small to medium sized devices.

Multitude of boards
-------------------

There is a multitude of modules and boards from different sources which carry
an i.MXRT chip.  MicroPython aims to provide a generic port which runs on
as many boards/modules as possible, but there may be limitations.  The
NXP IMXRT1020-EVK and the Teensy 4.0 and Teensy 4.1 development boards are taken
as reference for the port (for example, testing is performed on them).
For any board you are using please make sure you have a data sheet, schematics
and other reference materials so you can look up any board-specific functions.

The following boards are supported by the port:

- MIMXRT1010-EVK
- MIMXRT1020-EVK
- MIMXRT1050-EVK
- MIMXRT1060-EVK
- MIMXRT1064-EVK
- Teensy 4.0
- Teensy 4.1

Supported MCUs
--------------

+-------------+--------------------+-------------------------+
| Product     | CPU                | Memory                  |
+=============+====================+=========================+
| i.MX RT1064 | Cortex-M7 @600 MHz | 1 MB SRAM, 4 MB Flash   |
+-------------+--------------------+-------------------------+
| i.MX RT1061 | Cortex-M7 @600 MHz | 1 MB SRAM               |
+-------------+--------------------+-------------------------+
| i.MX RT1062 | Cortex-M7 @600 MHz | 1 MB SRAM               |
+-------------+--------------------+-------------------------+
| i.MX RT1050 | Cortex-M7 @600 MHz | 512 kB SRAM             |
+-------------+--------------------+-------------------------+
| i.MX RT1020 | Cortex-M7 @500 MHz | 256 kB SRAM             |
+-------------+--------------------+-------------------------+
| i.MX RT1010 | Cortex-M7 @500 MHz | 128 kB SRAM             |
+-------------+--------------------+-------------------------+

Note: Most of the controllers do not have internal flash memory. Therefore
their flash capacity is dependent on an external flash chip.

To make a generic MIMXRT port and support as many boards as possible the
following design and implementation decision were made:

* GPIO pin numbering is based on the board numbering as well as on the
  MCU numbering.  Please have the manual/pin diagram of your board at hand
  to find correspondence between your board pins and actual i.MXRT pins.
* All MCU pins are supported by MicroPython but not all are usable on any given board.

Technical specifications and SoC datasheets
-------------------------------------------

The data sheets and other reference material for i.MXRT chip are available
from the vendor site: https://www.nxp.com/products/processors-and-microcontrollers/arm-microcontrollers/i-mx-rt-crossover-mcus:IMX-RT-SERIES .
They are the primary reference for the chip technical specifications, capabilities,
operating modes, internal functioning, etc.

For your convenience, a few technical specifications are provided below:

* Architecture: ARM Cortex M7
* CPU frequency: up to 600MHz
* Total RAM available: up to 1 MByte (see table)
* BootROM: 96KB
* External FlashROM: code and data, via SPI Flash; usual size 2 - 8 MB
  Some boards provide additional external RAM and SPI flash.
* GPIO: up to 124 (GPIOs are multiplexed with other functions, including
  external FlashROM, UART, etc.)
* UART: 4 or 8 RX/TX UART. Hardware handshaking is supported by the MCU,
  but the boards used for testing do not expose the signals.
* SPI: 2 or 4 low power SPI interfaces (software implementation available on every pin)
* I2C: 2 or 4 low power I2C interfaces (software implementation available on every pin)
* I2S: 3 I2S interfaces
* ADC: one or two 12-bit SAR ADC converters
* Ethernet controller
* Programming: using BootROM bootloader from USB - due to external FlashROM
  and always-available BootROM bootloader, the MIMXRT is not brickable

The lower numbers for UART, SPI and I2C apply to the i.MXRT 101x MCU.

For more information see the i.MXRT data sheets or reference manuals.
NXP provides software support through it's SDK packages.
