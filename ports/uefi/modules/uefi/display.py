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

# uefi.display — the GOP display as a MicroPython framebuf.FrameBuffer.
#
# framebuf's only colour format is RGB565 (16bpp), while GOP wants 32bpp BGRx, so
# a Display keeps an RGB565 back-buffer (the one framebuf draws into) and, on
# show(), expands it to BGRx (via the C raw.convert_rgb565) and Blts it. Draw with
# any framebuf method on `.fb`, then call `.show()`.

import framebuf
import uctypes

from . import raw
from .protocols import GOP, GOP_MODE, GOP_INFO


def rgb565(r, g, b):
    """Pack 8-bit r,g,b into a framebuf RGB565 colour."""
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)


class Display:
    """A GOP display exposed as a framebuf.FrameBuffer.

    `display.fb`     — the framebuf.FrameBuffer (RGB565); use its draw methods.
    `display.modes()`— [(mode, width, height), ...] of every supported resolution.
    `display.set_mode(n)` — switch resolution (rebuilds the buffers).
    `display.show()` — push the framebuffer to the screen.
    """

    def __init__(self, mode=None):
        self.gop = GOP.locate()  # raises uefi.Error if there's no display
        if mode is not None:
            self.gop.set_mode(mode)
        self._setup()

    def _setup(self):
        info = self.gop.struct_at("mode", GOP_MODE).struct_at("info", GOP_INFO)
        self.width = info.horizontal_resolution
        self.height = info.vertical_resolution
        self._n = self.width * self.height
        self._src565 = bytearray(self._n * 2)  # framebuf draws here
        self._dstbgrx = bytearray(self._n * 4)  # expanded for Blt
        self._sp = uctypes.addressof(self._src565)
        self._dp = uctypes.addressof(self._dstbgrx)
        self.fb = framebuf.FrameBuffer(self._src565, self.width, self.height, framebuf.RGB565)

    def modes(self):
        """List (mode_number, width, height) for every supported display mode."""
        m = self.gop.struct_at("mode", GOP_MODE)
        out = []
        size = bytearray(8)  # UINTN SizeOfInfo (out)
        ip = bytearray(8)  # EFI_..._INFO * Info (out; firmware-allocated)
        for i in range(m.max_mode):
            self.gop.query_mode(i, size, ip)
            p = int.from_bytes(ip, "little")
            gi = GOP_INFO.wrap(p)
            out.append((i, gi.horizontal_resolution, gi.vertical_resolution))
            raw.free_pool(p)  # QueryMode allocates the info buffer; we own it
        return out

    def set_mode(self, mode):
        self.gop.set_mode(mode)
        self._setup()

    def show(self):
        raw.convert_rgb565(self._sp, self._dp, self._n)
        self.gop.blt(self._dp, 2, 0, 0, 0, 0, self.width, self.height, 0)  # 2 = BufferToVideo
