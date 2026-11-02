# SPDX-FileCopyrightText: 2026 Mikey Sklar for Adafruit Industries
#
# SPDX-License-Identifier: MIT

# Copy to CIRCUITPY with code.py from this directory. Pins are for the CLUE.
import board
import busdisplay
import busio
import displayio
import fourwire
import supervisor

displayio.release_displays()
spi = busio.SPI(board.TFT_SCK, board.TFT_MOSI)
bus = fourwire.FourWire(
    spi,
    command=board.TFT_DC,
    chip_select=board.TFT_CS,
    reset=board.TFT_RESET,
    baudrate=60000000,
)
INIT = b"\x01\x80\x96\x11\x80\xff\x36\x01\xa0\x3a\x01\x55\x21\x80\x0a\x13\x80\x0a\x29\x80\xff"
supervisor.runtime.display = busdisplay.BusDisplay(
    bus, INIT, width=240, height=240, colstart=80, rotation=90, backlight_pin=board.TFT_BACKLIGHT
)
