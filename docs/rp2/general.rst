.. _rp2_general:

General information about the RP2xxx port
=========================================

The rp2 port supports boards powered by the Raspberry Pi Foundation's RP2xxx
family of microcontrollers, most notably the Raspberry Pi Pico that employs
the RP2040.

Technical specifications and SoC datasheets
-------------------------------------------

For detailed technical specifications, please refer to the `datasheet <https://datasheets.raspberrypi.org/rp2040/rp2040-datasheet.pdf>`_

The RP2040 microcontroller a 40 nm silicon in a 7X7mm QFN-56 SMD package.The key features include:

# 133 MHz dual ARM Cortex-M0+ cores (overclockable to over 400 MHz)
	* Each core has an integer divider peripheral and two interpolators
# 264KB SRAM in six independent banks
# No internal Flash or EEPROM memory (after reset, the bootloader loads firmware from either the external flash memory or USB bus into internal SRAM)
# QSPI bus controller, which supports up to 16 MB of external Flash memory
# DMA Controller
# AHB crossbar, fully-connected
# On-chip programmable LDO to generate core voltage
# 2 on-chip PLLs to generate USB and core clocks
# 30 GPIO pins, of which 4 can optionally be used as analog inputs

The peripherals include 2 UARTs, 2 SPI controllers, 2 I2C contollers, 16 PWM channels, USB 1.1 controller and PHY, with host and device support, 8 PIO state machines.

RP2040 is the debut microcontroller from Raspberry Pi from the Raspberry Pi Foundation.

It has large on-chip memory, symmetric dual-core processor complex, deterministic bus fabric, and rich peripheral set augmented with  unique Programmable I/O (PIO) subsystem, providing professional users with unrivalled power and flexibility. It has the lowest possible barrier to entry for beginners because of its polished MicroPython port and a UF2 bootloader in ROM.

RP2040 is a stateless device, with support for cached execute-in-place from external QSPI memory. This gives us the flexibility to choose the appropriate density of non-volatile storage for the application, and the additional benefit of low pricing of commodity Flash parts.

Manufactured on a modern 40nm process node, the RP2040 delivers high performance, low dynamic power consumption, and low leakage, with support for extended-duration operation on battery power through a variety of low-power operations.

