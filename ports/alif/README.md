MicroPython port to Alif Ensemble MCUs
======================================

This is a port of MicroPython to the Alif Ensemble series of microcontrollers.

Initial development of this Alif port was sponsored by OpenMV LLC.

## Basic Features:
* pin support, named pins, AFs, find_pin etc..
* pendsv/systick drivers, dispatch etc..
* machine_(uart/spi/i2c/led).
* Octal SPI Flash.- VFS/FatFS/storage support.
* TinyUSB support CDC + MSC device support (MSC will use the OSPI flash).
* Bluetooth + wifi using CYW43 in SPI mode.
* More machine modules ?

## The following "advanced features" will follow later:
* Ethernet support.
* Dual core support HE/HP.
* SDRAM support.
* Deepsleep/low power mode.
* Other machine modules.
