# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# EFI_PCI_IO_PROTOCOL — access to a PCI controller (config space, BAR I/O, DMA).
#
# The function-pointer slots must be declared in order so the offsets line up; each
# signature follows MdePkg/Include/Protocol/PciIo.h. pci_read/write: (Width:u32,
# Offset:u32, Count:UINTN, Buffer). get_location writes four UINTNs: Segment, Bus,
# Device, Function. poll_mem/poll_io: (Width, BarIndex, Offset, Mask, Value,
# Delay in 100 ns units) -> the last value read; EFI_TIMEOUT if it never matched.

from ..protocol import ProtocolDescriptor, Field, Method, Out, U8, U32, U64, VPtr
from .. import guid

PCI_IO = ProtocolDescriptor(
    "EFI_PCI_IO_PROTOCOL",
    guid.PCI_IO_PROTOCOL,
    [
        Method("poll_mem", (U32, U8, U64, U64, U64, U64, Out(U64))),
        Method("poll_io", (U32, U8, U64, U64, U64, U64, Out(U64))),
        Method("mem_read", (U32, U8, U64, U64, VPtr)),
        Method("mem_write", (U32, U8, U64, U64, VPtr)),
        Method("io_read", (U32, U8, U64, U64, VPtr)),
        Method("io_write", (U32, U8, U64, U64, VPtr)),
        Method("pci_read", (U32, U32, U64, VPtr)),
        Method("pci_write", (U32, U32, U64, VPtr)),
        Method("copy_mem", (U32, U8, U64, U8, U64, U64)),
        Method("map", (U32, VPtr, VPtr, VPtr, VPtr)),
        Method("unmap", (VPtr,)),
        Method("allocate_buffer", (U32, U32, U64, VPtr, U64)),
        Method("free_buffer", (U64, VPtr)),
        Method("flush", ()),
        # GetLocation writes four UINTNs; as Out slots it returns them decoded.
        Method("get_location", (Out(U64), Out(U64), Out(U64), Out(U64))),
        Method("attributes", (U32, U64, VPtr)),
        Method("get_bar_attributes", (U8, VPtr, VPtr)),
        Method("set_bar_attributes", (U64, U8, VPtr, VPtr)),
        Field("rom_size", U64),
        Field("rom_image", VPtr),
    ],
)
