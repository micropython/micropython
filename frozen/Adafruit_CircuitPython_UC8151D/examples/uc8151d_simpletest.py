# SPDX-FileCopyrightText: 2017 Scott Shawcroft, written for Adafruit Industries
# SPDX-FileCopyrightText: Copyright (c) 2021 Melissa LeBlanc-Williams for Adafruit Industries
#
# SPDX-License-Identifier: Unlicense

"""Simple test script for 2.9" 296x128 monochrome display.

Supported products:
  * Adafruit Flexible 2.9" Monochrome
    * https://www.adafruit.com/product/4262
  """
# pylint: disable=no-member

import time
import board
import displayio
import adafruit_uc8151d

displayio.release_displays()

# This pinout works on a Feather M4 and may need to be altered for other boards.
spi = board.SPI()  # Uses SCK and MOSI
epd_cs = board.D9
epd_dc = board.D10
epd_reset = board.D5
epd_busy = None

display_bus = displayio.FourWire(
    spi, command=epd_dc, chip_select=epd_cs, reset=epd_reset, baudrate=1000000
)
time.sleep(1)

display = adafruit_uc8151d.UC8151D(
    display_bus, width=296, height=128, rotation=90, busy_pin=epd_busy
)

g = displayio.Group()

with open("/display-ruler.bmp", "rb") as f:
    pic = displayio.OnDiskBitmap(f)
    t = displayio.TileGrid(pic, pixel_shader=pic.pixel_shader)
    g.append(t)

    display.show(g)

    display.refresh()

    time.sleep(120)
