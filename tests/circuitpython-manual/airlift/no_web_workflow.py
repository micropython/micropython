# SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
#
# SPDX-License-Identifier: MIT
"""With no CIRCUITPY_WEB_API_PASSWORD in settings.toml, the core must leave the
ESP32 alone, so ESP32SPI code that passes its own pins works as before.
Remove the password, reset the board, then run this with run_on_board.py
(or as code.py). Expected: "core owns the ESP32: False" and a firmware version."""

import _airlift
import board
import busio
from digitalio import DigitalInOut

from adafruit_esp32spi import adafruit_esp32spi

print("core owns the ESP32:", _airlift.active())
esp = adafruit_esp32spi.ESP_SPIcontrol(
    busio.SPI(board.SCK, board.MOSI, board.MISO),
    DigitalInOut(board.ESP_CS),
    DigitalInOut(board.ESP_BUSY),
    DigitalInOut(board.ESP_RESET),
)
print("old-style ESP_SPIcontrol firmware:", esp.firmware_version)
print("status after its own reset:", esp.status)
