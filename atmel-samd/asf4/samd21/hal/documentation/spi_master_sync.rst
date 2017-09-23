The SPI Master Synchronous Driver
=================================

The serial peripheral interface (SPI) is a synchronous serial communication
interface.

SPI devices communicate in full duplex mode using a master-slave
architecture with a single master. The master device originates the frame for
reading and writing. Multiple slave devices are supported through selection
with individual slave select (SS) lines.

Features
--------

* Initialization/de-initialization
* Enabling/disabling
* Control of the following settings:

  * Baudrate
  * SPI mode
  * Character size
  * Data order
* Data transfer: transmission, reception and full-duplex

Applications
------------

Send/receive/exchange data with a SPI slave device. E.g., serial flash, SD card,
LCD controller, etc.

Dependencies
------------

SPI master capable hardware

Concurrency
-----------

N/A

Limitations
-----------

The slave select (SS) is not automatically inserted during read/write/transfer,
user must use I/O to control the devices' SS.

Known issues and workarounds
----------------------------

N/A

