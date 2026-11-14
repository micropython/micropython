# MIT license; Copyright (c) 2026 OpenMV LLC.
#
# Test SPI with an SSD1351 128x128 RGB OLED, such as the one on the OpenMV AE3
# carrier board.
#
# This sends full frames over SPI, checking that the transfers complete and
# measuring the frame rate.  The display output must be checked visually: it
# shows solid red, green, blue, white, then vertical colour bars.
#
# The display is configured by `ssd1351_display`, see tests/target_wiring/README.md.
#
# requires: target-wiring

import time
from machine import Pin, SPI

try:
    from target_wiring import ssd1351_display
except ImportError:
    print("SKIP")
    raise SystemExit

WIDTH = 128
HEIGHT = 128
BAUDRATE = 20_000_000

spi = SPI(ssd1351_display["spi"], baudrate=BAUDRATE, polarity=0, phase=0)
cs = Pin(ssd1351_display["cs"], Pin.OUT, value=1)
dc = Pin(ssd1351_display["dc"], Pin.OUT, value=1)
rst = Pin(ssd1351_display["rst"], Pin.OUT, value=1)


def write(cmd, data=None):
    cs(0)
    dc(0)
    spi.write(bytes((cmd,)))
    if data is not None:
        dc(1)
        spi.write(data)
    cs(1)


# Hardware reset.
rst(0)
time.sleep_ms(100)
rst(1)
time.sleep_ms(100)

# Init sequence (see the SSD1351 datasheet).
write(0xFD, b"\x12")  # unlock MCU interface
write(0xFD, b"\xb1")  # unlock commands A2,B1,B3,BB,BE,C1
write(0xAE)  # display off
write(0xB2, b"\xa4\x00\x00")  # display enhancement
write(0xB3, b"\xf0")  # clock divider
write(0xCA, b"\x7f")  # mux ratio
write(0xA0, b"\x74")  # remap: RGB565, colour order
write(0xA2, b"\x00")  # display offset
write(0xB1, b"\x32")  # precharge
write(0xBB, b"\x1f")  # precharge level
write(0xC7, b"\x0a")  # master contrast
write(0xC1, b"\xff\xff\xff")  # contrast A, B, C
write(0xB6, b"\x01")  # second precharge
write(0x15, b"\x00\x7f")  # column range
write(0x75, b"\x00\x7f")  # row range
write(0xAF)  # display on

frame = bytearray(WIDTH * HEIGHT * 2)


def fill(color):
    hi = color >> 8
    lo = color & 0xFF
    for i in range(0, len(frame), 2):
        frame[i] = hi
        frame[i + 1] = lo


def show():
    write(0x5C, frame)


colors = (("red", 0xF800), ("green", 0x07E0), ("blue", 0x001F), ("white", 0xFFFF))
for name, color in colors:
    fill(color)
    show()
    print("fill", name)
    time.sleep_ms(300)

# Vertical colour bars.
bars = (0xF800, 0x07E0, 0x001F, 0xFFE0, 0xF81F, 0x07FF, 0xFFFF, 0x0000)
row = bytearray(WIDTH * 2)
for x in range(WIDTH):
    c = bars[x * len(bars) // WIDTH]
    row[2 * x] = c >> 8
    row[2 * x + 1] = c & 0xFF
for y in range(HEIGHT):
    frame[y * WIDTH * 2 : (y + 1) * WIDTH * 2] = row
show()
print("bars")

# Frame rate, mostly bound by the SPI clock.
n = 20
t0 = time.ticks_ms()
for _ in range(n):
    show()
dt = time.ticks_diff(time.ticks_ms(), t0)
fps = n * 1000 // dt
# A full frame is 256 kbit, so at 20MHz the upper bound is ~76 fps.
print("fps", 30 <= fps <= 80)

spi.deinit()
