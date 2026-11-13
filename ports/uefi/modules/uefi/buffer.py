# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# uefi.buffer — UEFI pool-backed memory (BS->AllocatePool / FreePool), and
# page-aligned memory (BS->AllocatePages / FreePages) as PageBuffer.
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

# EFI_ALLOCATE_TYPE values.
ALLOCATE_ANY_PAGES = 0
ALLOCATE_MAX_ADDRESS = 1
ALLOCATE_ADDRESS = 2
PAGE_SIZE = 4096


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

    def _free(self):
        raw.free_pool(self.ptr)

    def close(self):
        if self.ptr:
            self._free()
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
        return "%s(ptr=0x%x, size=%d)" % (type(self).__name__, self.ptr, self.size)


class PageBuffer(PoolBuffer):
    """Page-aligned memory from AllocatePages: `size` bytes rounded up to whole pages.

    By default the pages may be anywhere. `max_address` keeps the whole buffer at or
    below that address (e.g. 0xFFFF_FFFF for a device limited to 32-bit DMA), and
    `address` asks for exactly that physical address.
    """

    def __init__(self, size, memory_type=BOOT_SERVICES_DATA, max_address=None, address=None):
        if max_address is not None and address is not None:
            raise ValueError("give max_address or address, not both")
        if size < 0:
            raise ValueError("negative size")
        if address is not None:
            kind, addr = ALLOCATE_ADDRESS, address
        elif max_address is not None:
            kind, addr = ALLOCATE_MAX_ADDRESS, max_address
        else:
            kind, addr = ALLOCATE_ANY_PAGES, 0
        pages = max(1, (size + PAGE_SIZE - 1) // PAGE_SIZE)
        st, ptr = raw.allocate_pages(kind, memory_type, pages, addr)
        status.check(st)
        self.ptr = ptr
        self.pages = pages
        self.size = pages * PAGE_SIZE
        self.view = uctypes.bytearray_at(ptr, self.size)

    def _free(self):
        raw.free_pages(self.ptr, self.pages)
