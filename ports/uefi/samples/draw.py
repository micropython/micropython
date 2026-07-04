# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2026 Nicko van Someren
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# draw.py — open the Graphics Output (GOP) display and draw lines + circles.
#
# Run with a display attached:  make run-gfx
# Then at the REPL:             exec(open('/samples/draw.py').read())
#
# A back-buffer is drawn into and pushed to the screen with one Blt, so it works
# on any GOP (linear framebuffer or BltOnly) — Blt handles pixel format + stride.

import uefi
import uctypes


class Canvas:
    """Off-screen RGB canvas pushed to the GOP display. Colours are 0xRRGGBB."""

    def __init__(self):
        gop = uefi.protocols.GOP.locate()
        info = gop.struct_at("mode", uefi.protocols.GOP_MODE).struct_at(
            "info", uefi.protocols.GOP_INFO
        )
        self.gop = gop
        self.w = info.horizontal_resolution
        self.h = info.vertical_resolution
        self.buf = bytearray(self.w * self.h * 4)  # BGRx back-buffer (black)
        self._wrap()

    def _wrap(self):
        self.addr = uctypes.addressof(self.buf)
        self.px = uctypes.struct(
            self.addr,
            {"a": (uctypes.ARRAY | 0, uctypes.UINT32 | (self.w * self.h))},
            uctypes.LITTLE_ENDIAN,
        ).a

    def pixel(self, x, y, color):
        if 0 <= x < self.w and 0 <= y < self.h:
            self.px[y * self.w + x] = color  # 0xRRGGBB == BGRx little-endian

    def fill(self, color=0x000000):  # fast C-level byte repeat
        p = bytes((color & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF, 0))
        self.buf = bytearray(p * (self.w * self.h))
        self._wrap()

    def flush(self):  # 2 = EfiBltBufferToVideo
        self.gop.blt(self.addr, 2, 0, 0, 0, 0, self.w, self.h, 0)

    def line(self, x0, y0, x1, y1, color):  # Bresenham
        dx = abs(x1 - x0)
        dy = -abs(y1 - y0)
        sx = 1 if x0 < x1 else -1
        sy = 1 if y0 < y1 else -1
        err = dx + dy
        while True:
            self.pixel(x0, y0, color)
            if x0 == x1 and y0 == y1:
                break
            e2 = 2 * err
            if e2 >= dy:
                err += dy
                x0 += sx
            if e2 <= dx:
                err += dx
                y0 += sy

    def circle(self, cx, cy, r, color):  # midpoint circle
        x = r
        y = 0
        err = 1 - r
        while x >= y:
            for dx, dy in ((x, y), (y, x), (-y, x), (-x, y), (-x, -y), (-y, -x), (y, -x), (x, -y)):
                self.pixel(cx + dx, cy + dy, color)
            y += 1
            if err < 0:
                err += 2 * y + 1
            else:
                x -= 1
                err += 2 * (y - x) + 1


def wait_key():
    import sys

    sys.stdin.read(1)  # blocks on the WFE until a key (window or terminal); no echo


c = Canvas()
c.fill(0x000020)  # dark-blue background
c.line(0, 0, c.w - 1, c.h - 1, 0xFF0000)  # red diagonal
c.line(0, c.h - 1, c.w - 1, 0, 0x00FF00)  # green diagonal
cx, cy, r, i = c.w // 2, c.h // 2, 20, 0
while r < min(cx, cy):
    c.circle(cx, cy, r, 0xFFFF00 if i % 2 else 0x00FFFF)
    r += 40
    i += 1
c.flush()
print("drawn %dx%d — press a key..." % (c.w, c.h))
wait_key()  # keep the picture on screen
