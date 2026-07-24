# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# EFI_SERIAL_IO_PROTOCOL — UART access. Current settings live in a SERIAL_IO_MODE
# struct reached via the `mode` pointer: bio.struct_at("mode", SERIAL_IO_MODE).

from ..protocol import ProtocolDescriptor, Field, Method, U8, U32, U64, VPtr
from .. import guid

SERIAL_IO_MODE = ProtocolDescriptor(
    "EFI_SERIAL_IO_MODE",
    None,
    [
        Field("control_mask", U32),
        Field("timeout", U32),
        Field("baud_rate", U64),
        Field("receive_fifo_depth", U32),
        Field("data_bits", U32),
        Field("parity", U32),
        Field("stop_bits", U32),
    ],
)

SERIAL_IO = ProtocolDescriptor(
    "EFI_SERIAL_IO_PROTOCOL",
    guid.SERIAL_IO_PROTOCOL,
    [
        Field("revision", U32),
        Method("reset", ()),
        Method(
            "set_attributes", (U64, U32, U32, U32, U8, U32)
        ),  # baud, fifo, timeout, parity, databits, stopbits
        Method("set_control", (U32,)),
        Method("get_control", (VPtr,)),
        Method("write", (VPtr, VPtr)),  # (UINTN *BufferSize, void *Buffer)
        Method("read", (VPtr, VPtr)),
        Field("mode", VPtr),
    ],
)
