.. _esp32_general:

General information about the ESP32 port
==========================================

ESP32 is a popular WiFi-enabled System-on-Chip (SoC) by Espressif Systems.

Multitude of boards
-------------------

There is a multitude of modules and boards from different sources which carry
the ESP32 chip. MicroPython tries to provide a generic port which would run on
as many boards/modules as possible, but there may be limitations. Adafruit
Feather HUZZAH board is taken as a reference board for the port (for example,
testing is performed on it). If you have another board, please make sure you
have a datasheet, schematics and other reference materials for your board
handy to look up various aspects of your board functioning.

To make a generic ESP32 port and support as many boards as possible,
the following design and implementation decision were made:

* GPIO pin numbering is based on ESP32 chip numbering, not some "logical"
  numbering of a particular board. Please have the manual/pin diagram of your board
  at hand to find correspondence between your board pins and actual ESP32 pins.
  We also encourage users of various boards to share this mapping via MicroPython
  forum, with the idea to collect community-maintained reference materials
  eventually.
* All pins which make sense to support, are supported by MicroPython
  (for example, pins which are used to connect SPI flash
  are not exposed, as they're unlikely useful for anything else, and
  operating on them will lead to board lock-up). However, any particular
  board may expose only subset of pins. Consult your board reference manual.
* Some boards may lack external pins/internal connectivity to support
  ESP32 deepsleep mode.


Technical specifications and SoC datasheets
-------------------------------------------

The datasheets and other reference material for ESP32 chip are available
from the vendor site: https://www.espressif.com/en/support/download/documents?keys=esp32 .
They are the primary reference for the chip technical specifications, capabilities,
operating modes, internal functioning, etc.

For your convenience, some of technical specifications are provided below:

* Architecture: Xtensa Dual-Core 32-bit LX6
* CPU frequency: upto 240MHz
* Total RAM available: 528KB (part of it reserved for system)
* BootROM: 448KB
* Internal FlashROM: None
* External FlashROM: code and data, via SPI Flash. Normal sizes 4MB
* GPIO: 34 (GPIOs are multiplexed with other functions, including
  external FlashROM, UART, deep sleep wake-up, etc.)
* UART: 3 RX/TX UART (no hardware handshaking), one TX-only UART
* SPI: 4 SPI interfaces (one used for FlashROM)
* I2C: 2 I2C (bitbang implementation available on any pins)
* I2S: 2
* ADC: 12-bit SAR ADC up to 18 channels
* DAC: 2  8-bit DAC
* Programming: using BootROM bootloader from UART - due to external FlashROM
  and always-available BootROM bootloader, the ESP32 is not brickable
 
For more information see the ESP32 datasheet: https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf 

MicroPython is implemented on top of the ESP-IDF, Espressif's development 
framework for the ESP32. See the `ESP-IDF Programming Guide <https://docs.espressif.com/projects/esp-idf/en/latest/index.html>`_ 
for details.
