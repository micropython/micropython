# SPDX-FileCopyrightText: 2017 Scott Shawcroft, written for Adafruit Industries
# SPDX-FileCopyrightText: Copyright (c) 2021 Melissa LeBlanc-Williams for Adafruit Industries
#
# SPDX-License-Identifier: Unlicense

"""Simple test script for 2.13" 250x122 tri-color display.
Supported products:
  * Adafruit 2.13" Tri-Color eInk Display Breakout
    * https://www.adafruit.com/product/4947
  * Adafruit 2.13" Tri-Color eInk Display FeatherWing
    * https://www.adafruit.com/product/4814
"""

import time
import board
import displayio
import adafruit_ssd1680

displayio.release_displays()

# This pinout works on a Metro M4 and may need to be altered for other boards.
spi = board.SPI()  # Uses SCK and MOSI
epd_cs = board.D9
epd_dc = board.D10
epd_reset = board.D8  # Set to None for FeatherWing
epd_busy = board.D7  # Set to None for FeatherWing

display_bus = displayio.FourWire(
    spi, command=epd_dc, chip_select=epd_cs, reset=epd_reset, baudrate=1000000
)
time.sleep(1)

display = adafruit_ssd1680.SSD1680(
    display_bus,
    width=250,
    height=122,
    busy_pin=epd_busy,
    highlight_color=0xFF0000,
    rotation=270,
)

g = displayio.Group()

with open("/display-ruler.bmp", "rb") as f:
    pic = displayio.OnDiskBitmap(f)
    # CircuitPython 6 & 7 compatible
    t = displayio.TileGrid(
        pic, pixel_shader=getattr(pic, "pixel_shader", displayio.ColorConverter())
    )
    # CircuitPython 7 compatible only
    # t = displayio.TileGrid(pic, pixel_shader=pic.pixel_shader)
    g.append(t)

    display.show(g)

    display.refresh()

    print("refreshed")

    time.sleep(120)
