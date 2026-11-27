# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# EFI_CPU_IO2_PROTOCOL — firmware-arbitrated CPU memory and port I/O access
# (MdePkg/Include/Protocol/CpuIo2.h). Each slot is (Width, Address:u64, Count:UINTN,
# Buffer); Width is the EFI_CPU_IO_PROTOCOL_WIDTH enum: 0..3 = 8/16/32/64-bit, +4 the
# Fifo variant (one address, buffer advances), +8 the Fill variant (buffer fixed).

from ..protocol import ProtocolDescriptor, Method, U32, U64, VPtr
from .. import guid

CPU_IO2 = ProtocolDescriptor(
    "EFI_CPU_IO2_PROTOCOL",
    guid.CPU_IO2_PROTOCOL,
    [
        Method("mem_read", (U32, U64, U64, VPtr)),
        Method("mem_write", (U32, U64, U64, VPtr)),
        Method("io_read", (U32, U64, U64, VPtr)),
        Method("io_write", (U32, U64, U64, VPtr)),
    ],
)
