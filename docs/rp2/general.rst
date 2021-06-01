.. _rp2_general:

General information about the RP2xxx port
=========================================

The rp2 port supports boards powered by the Raspberry Pi Foundation's RP2xxx
family of microcontrollers, most notably the Raspberry Pi Pico that employs
the RP2040.

Technical specifications and SoC datasheets
-------------------------------------------

If you want a deatiled technical specification, then you can view the `datasheets <https://datasheets.raspberrypi.org/rp2040/rp2040-datasheet.pdf>`_ 

Talking specifically about the RP2040 port, its features include the chip being a 40 nm silicon in a 7 X 7 mm QFN-56 SMD package.They key features include-

# 133 MHz dual ARM Cortex-M0+ cores (overclockable to over 400 MHz)
	* Each core has an integer divider peripheral, and two interpolators.
# 264 KB SRAM in six independent banks
# No internal Flash or EEPROM memory (after reset, the boot-loader loads firmware from either external flash memory or USB bus into internal SRAM)
# QSPI bus controller, supporting up to 16 MB of external Flash memory
# DMA Controller
# AHB crossbar, fully-connected
# On-chip programmable LDO to generate core voltage
# 2 on-chip PLLs to generate USB and core clocks
# 30 GPIO pins, of which 4 can optionally be used as analog inputs

The peripherals include 2 UARTs, 2SPI controllers, 2 I2C contollers, 16 PWM channels, USB 1.1 controller and PHY, with host and device support, 8 PIO stats machines.

RP2040 is the debut microcontroller from Raspberry Pi. It brings our signature values of high performance, low cost,and ease of use to the microcontroller space.

With a large on-chip memory, symmetric dual-core processor complex, deterministic bus fabric, and rich peripheral set augmented with our unique Programmable I/O (PIO) subsystem, it provides professional users with unrivalled power and flexibility. With detailed documentation, a polished MicroPython port, and a UF2 bootloader in ROM, it has the lowest possible barrier to entry for beginner and hobbyist users.

RP2040 is a stateless device, with support for cached execute-in-place from external QSPI memory. This design decision allows you to choose the appropriate density of non-volatile storage for your application, and to benefit from the low pricing of commodity Flash parts.

RP2040 is manufactured on a modern 40nm process node, delivering high performance, low dynamic power
consumption, and low leakage, with a variety of low-power modes to support extended-duration operation on battery power.

