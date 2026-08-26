# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# uefi.buffer — UEFI pool-backed memory (BS->AllocatePool / FreePool).
#
# Use a PoolBuffer when memory must survive Python GC without pinning, be handed
# to firmware that retains the pointer past a call, or back a two-phase size
# probe. The firmware token is exposed as `.ptr` (the .ptr contract) and via
# int(), so a PoolBuffer can be passed straight to uefi.raw calls; `.view` is a
# bytearray over the same memory, so it also works anywhere bytes do.

import uctypes

from . import raw
from . import status
from . import coerce

# EFI_MEMORY_TYPE values (efi.h enum order).
LOADER_DATA = 2
BOOT_SERVICES_DATA = 4


class PoolBuffer:
    def __init__(self, size, memory_type=BOOT_SERVICES_DATA):
        st, ptr = raw.allocate_pool(memory_type, size)
        status.check(st)
        self.ptr = ptr
        self.size = size
        self.view = uctypes.bytearray_at(ptr, size)

    @classmethod
    def from_bytes(cls, data, memory_type=BOOT_SERVICES_DATA):
        """Allocate a pool buffer and copy `data` into it.

        `data` is any byte block — bytes, a memoryview, another PoolBuffer, or an
        object that serialises itself such as a DevicePath.
        """
        data = coerce.as_buffer(data)
        buf = cls(len(data) or 1, memory_type)
        if data:
            buf.write(data)
        return buf

    def write(self, data, offset=0):
        """Copy any byte block into the buffer at `offset`."""
        data = coerce.as_buffer(data)
        if offset + len(data) > self.size:
            raise ValueError("write past end of buffer")
        self.view[offset : offset + len(data)] = data

    def bytes(self, offset=0, size=None):
        """Read `size` bytes (default: to the end) out of the buffer as bytes."""
        if size is None:
            size = self.size - offset
        return uctypes.bytes_at(self.ptr + offset, size)

    def __int__(self):
        # The `.ptr` contract in integer form (see uefi.handle.Handle).
        return self.ptr

    def __len__(self):
        return self.size

    def __getitem__(self, index):
        return self.view[index]

    def __setitem__(self, index, value):
        self.view[index] = value

    def close(self):
        if self.ptr:
            raw.free_pool(self.ptr)
            self.ptr = 0
            self.size = 0
            self.view = memoryview(b"")

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.close()

    def __del__(self):
        self.close()

    def __repr__(self):
        return "PoolBuffer(ptr=0x%x, size=%d)" % (self.ptr, self.size)
