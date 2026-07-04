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

# uefi.buffer — UEFI pool-backed memory (BS->AllocatePool / FreePool).
#
# Use a PoolBuffer when memory must survive Python GC without pinning, be handed
# to firmware that retains the pointer past a call, or back a two-phase size
# probe. The firmware token is exposed as `.ptr` (the .ptr contract), so a
# PoolBuffer can be passed straight to uefi.raw calls.

import uctypes

from . import raw
from . import status

# EFI_MEMORY_TYPE values (efi.h enum order).
LOADER_DATA = 2
BOOT_SERVICES_DATA = 4


class PoolBuffer:
    def __init__(self, size, memory_type=BOOT_SERVICES_DATA):
        st, ptr = raw.allocate_pool(memory_type, size)
        status.check(st)
        self.ptr = ptr
        self.size = size

    @classmethod
    def from_bytes(cls, data, memory_type=BOOT_SERVICES_DATA):
        """Allocate a pool buffer and copy `data` into it."""
        buf = cls(len(data) or 1, memory_type)
        if data:
            buf.write(data)
        return buf

    def write(self, data, offset=0):
        """Copy `data` into the buffer at `offset`."""
        if offset + len(data) > self.size:
            raise ValueError("write past end of buffer")
        dst = uctypes.bytearray_at(self.ptr + offset, len(data))
        for i in range(len(data)):
            dst[i] = data[i]

    def bytes(self, offset=0, size=None):
        """Read `size` bytes (default: to the end) out of the buffer as bytes."""
        if size is None:
            size = self.size - offset
        return uctypes.bytes_at(self.ptr + offset, size)

    def close(self):
        if self.ptr:
            raw.free_pool(self.ptr)
            self.ptr = 0
            self.size = 0

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.close()

    def __del__(self):
        self.close()

    def __repr__(self):
        return "PoolBuffer(ptr=0x%x, size=%d)" % (self.ptr, self.size)
