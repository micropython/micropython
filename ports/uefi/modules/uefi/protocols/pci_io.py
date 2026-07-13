# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# EFI_PCI_IO_PROTOCOL — access to a PCI controller (config space, BAR I/O, DMA).
#
# The interface is large; the function-pointer slots must be declared in order so
# the offsets line up. We give real signatures to the slots worth calling
# (get_location, pci_read/pci_write); the others are placeholders for spacing.
# pci_read/write: (Width:u32, Offset:u32, Count:UINTN, Buffer). get_location writes
# four UINTNs: Segment, Bus, Device, Function.

from ..protocol import ProtocolDescriptor, Field, Method, U8, U32, U64, VPtr
from .. import guid

PCI_IO = ProtocolDescriptor(
    "EFI_PCI_IO_PROTOCOL",
    guid.PCI_IO_PROTOCOL,
    [
        Method("poll_mem", (U32, U8, U64, U64, U64, VPtr)),
        Method("poll_io", (U32, U8, U64, U64, U64, VPtr)),
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
        Method("get_location", (VPtr, VPtr, VPtr, VPtr)),  # *Segment, *Bus, *Device, *Function
        Method("attributes", (U32, U64, VPtr)),
        Method("get_bar_attributes", (U8, VPtr, VPtr)),
        Method("set_bar_attributes", (U64, U8, VPtr, VPtr)),
        Field("rom_size", U64),
        Field("rom_image", VPtr),
    ],
)
