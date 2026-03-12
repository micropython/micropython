.. _rp2_general:

General information about the RP2xxx port
=========================================

The rp2 port supports boards powered by the Raspberry Pi Foundation's RP2xxx
family of microcontrollers, including the RP2040 and RP2350.

Technical specifications and SoC datasheets
-------------------------------------------

RP2040
^^^^^^

The RP2040 microcontroller is manufactured on a 40 nm silicon process in a 7x7mm
QFN-56 SMD package.

The key features include:

* 133 MHz dual ARM Cortex-M0+ cores (overclockable to over 400 MHz)
* 264KB SRAM in six independent banks
* No internal Flash or EEPROM memory (after reset, the bootloader loads
  firmware from either the external flash memory or USB bus into internal SRAM)
* QSPI bus controller, which
  supports up to 16 MB of external Flash memory
* On-chip programmable LDO to generate core voltage
* 2 on-chip PLLs to generate USB and core clocks
* 30 GPIO pins, of which 4 can optionally be used as analog inputs

The peripherals include:

* 2 UARTs
* 2 SPI controllers
* 2 I2C controllers
* 16 PWM channels
* USB 1.1 controller
* 8 PIO state machines

For detailed technical specifications, please refer to the `rp2040-datasheet
<https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf>`_

RP2350
^^^^^^

The RP2350 microcontroller is manufactured on a 40 nm silicon process and is available in
QFN-60 (RP2350A) or QFN-80 (RP2350B) packages.

The key features include:

* Dual-core Arm Cortex-M33 or Hazard3 RISC-V processors at up to 150MHz 
* 520KB on-chip SRAM 
* Support for up to 16MB of off-chip Flash memory via QSPI 
* USB 2.0 Full-Speed/Low-Speed controller 
* 30 GPIO pins (RP2350A) or 48 GPIO pins (RP2350B) 

The peripherals include:

* 2 UARTs 
* 2 SPI controllers 
* 2 I2C controllers 
* 24 PWM channels 
* 12 PIO state machines 
* HSTX high-speed transmitter 

For detailed technical specifications, please refer to the `rp2350-datasheet
<https://datasheets.raspberrypi.com/rp2350/rp2350-datasheet.pdf>`_ 
