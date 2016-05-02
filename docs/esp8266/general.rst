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
