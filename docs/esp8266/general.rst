General information about the ESP8266 port
==========================================

ESP8266 is a popular WiFi-enabled System-on-Chip (SoC) by Espressif Systems.

Multitude of boards
-------------------

There are multitude of modules and boards from different sources which carry
ESP8266 chip. MicroPython tries to provide a generic port which would run on
as many boards/modules as possible, but there may be limitations. Adafruit
Feather HUZZAH board is taken as a reference board for the port (for example,
testing is performed on it). If you have another board, please make sure you
have datasheet, schematics and other reference materials for your board
handy to look up various aspects of your board functioning.

To make a generic ESP8266 port and support as many boards as possible,
following design and implementation decision were made:

* GPIO pin numbering is based on ESP8266 chip numbering, not some "logical"
  numbering of a particular board. Please have manual/pin diagram of your board
  handy to find correspondce between your board pins and actual ESP8266 pins.
  We also encourage users of various boards to share this mapping via MicroPython
  forum, with the idea to collect community-maintained reference materials
  eventually.
* All pins which make sense to support, are supported by MicroPython
  (for example, we don't expose pins which are used to connect SPI flash
  are not exposed, as they're unlikely useful for anything else, and
  operating on them will lead to board lock-up). However, any particular
  board may expose only subset of pins. Consult your board reference manual.
* Some boards may lack external pins/internal connectivity to support
  ESP8266 deepsleep mode.


Technical specifications and SoC datasheets
-------------------------------------------

The datasheets and other reference material for ESP8266 chip are available
from the vendor site: http://bbs.espressif.com/viewtopic.php?f=67&t=225 .
The are primary reference for the chip technical specifications, capabilities,
operating modes, internal functioning, etc.

For your convinience, some of technical specifications are provided below:

* Architecture: Xtensa lx106
* CPU frequency: 80MHz overclockable to 160MHz
* Total RAM available: 96KB (part of it reserved for system)
* BootROM: 64KB
* Internal FlashROM: None
* External FlashROM: code and data, via SPI Flash. Normal sizes 512KB-4MB.
* GPIO: 16 + 1 (GPIOs are multiplexed with other functions, including
  external FlashROM, UART, deep sleep wake-up, etc.)
* UART: One RX/TX UART (no hardware handshaking), one TX-only UART.
* SPI: 2 SPI interfaces (one used for FlashROM).
* I2C: No native extenal I2C (bitbang implementation available on any pins).
* I2S: 1.
* Programming: using BootROM bootloader from UART. Due to external FlashROM
  and always-available BootROM bootloader, ESP8266 is not brickable.
