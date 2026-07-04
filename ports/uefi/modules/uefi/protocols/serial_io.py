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
