.. _rp2_general:

General information about the RP2xxx port
=========================================

The rp2 port supports boards powered by the Raspberry Pi Foundation's RP2xxx
family of microcontrollers, most notably the Raspberry Pi Pico that employs
the RP2040.

Technical specifications and SoC datasheets
-------------------------------------------

For detailed technical specifications, please refer to the `datasheets
<https://datasheets.raspberrypi.org/rp2040/rp2040-datasheet.pdf>`_

The RP2040 microcontroller is manufactured on a 40 nm silicon process in a 7x7mm
QFN-56 SMD package. The key features include:

* 133 MHz dual ARM Cortex-M0+ cores (overclockable to over 400 MHz)
* 264KB SRAM in six independent banks
* No internal Flash or EEPROM memory (after reset, the bootloader loads
  firmware from either the external flash memory or USB bus into internal SRAM)
* QSPI bus controller, which
  supports up to 16 MB of external Flash memory
* On-chip programmable LDO togenerate core voltage
* 2 on-chip PLLs to generate USB and core clocks
* 30 GPIOpins, of which 4 can optionally be used as analog inputs

The peripherals include:

* 2 UARTs
* 2 SPI controllers
* 2 I2C contollers
* 16 PWM channels
* USB 1.1 controller
* 8 PIO state machines
