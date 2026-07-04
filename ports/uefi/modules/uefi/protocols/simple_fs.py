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

# EFI_SIMPLE_FILE_SYSTEM_PROTOCOL — opens the root directory of a FAT volume.
# open_volume(out_root_pp) writes an EFI_FILE_PROTOCOL* into the buffer at the
# given pointer; pass a PoolBuffer (or other .ptr-bearing 8-byte region).

from ..protocol import ProtocolDescriptor, Field, Method, U64, VPtr
from .. import guid

SIMPLE_FS = ProtocolDescriptor(
    "EFI_SIMPLE_FILE_SYSTEM_PROTOCOL",
    guid.SIMPLE_FILE_SYSTEM_PROTOCOL,
    [
        Field("revision", U64),
        Method("open_volume", (VPtr,)),  # OpenVolume(**Root)
    ],
)
