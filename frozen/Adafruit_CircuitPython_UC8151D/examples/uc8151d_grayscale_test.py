# SPDX-FileCopyrightText: 2022 Martin Refseth, written for Adafruit Industries
#
# SPDX-License-Identifier: Unlicense

"""Simple test script for 1.54" 152x152 grayscale display.

Supported products:
  * 1.54" Grayscale Display (GDEW0154T8D)
  """
# pylint: disable=no-member

import time
import board
import displayio
import busio
import adafruit_uc8151d

displayio.release_displays()

# Pinout intended for use with a Raspberry Pi Pico
clk = board.GP10
si = board.GP11
dc = board.GP8
cs = board.GP9
rst = board.GP12
busy = board.GP13

display_bus = displayio.FourWire(
    busio.SPI(clk, si), command=dc, chip_select=cs, reset=rst, baudrate=1000000
)

time.sleep(1)

display = adafruit_uc8151d.UC8151D(
    display_bus, width=152, height=152, busy_pin=busy, rotation=180, grayscale=True
)


bitmap = displayio.Bitmap(152, 152, 4)

# Draw Black
for x in range(0, 152):
    for y in range(0, 38):
        bitmap[x, y] = 0
# Draw Dark Gray
for x in range(0, 152):
    for y in range(38, 76):
        bitmap[x, y] = 1
# Draw Light Gray
for x in range(0, 152):
    for y in range(76, 114):
        bitmap[x, y] = 2
# Draw White
for x in range(0, 152):
    for y in range(114, 152):
        bitmap[x, y] = 3

palette = displayio.Palette(4)
palette[0] = 0x000000  # Black
palette[1] = 0x404040  # Dark Gray
palette[2] = 0x808080  # Light Gray
palette[3] = 0xFFFFFF  # White

g = displayio.Group()
t = displayio.TileGrid(bitmap, pixel_shader=palette)
g.append(t)
display.show(g)
display.refresh()
