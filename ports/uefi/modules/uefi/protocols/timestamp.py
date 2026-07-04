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

# EFI_TIMESTAMP_PROTOCOL — a high-resolution monotonic counter. Like the device-
# path text protocols, its methods take NO `This` pointer (this=False).
# get_timestamp() returns the raw UINT64 count (not a status), so it is check=False.

from ..protocol import ProtocolDescriptor, Method, VPtr
from .. import guid

TIMESTAMP = ProtocolDescriptor(
    "EFI_TIMESTAMP_PROTOCOL",
    guid.TIMESTAMP_PROTOCOL,
    [
        Method("get_timestamp", (), check=False, this=False),  # -> UINT64 count
        Method("get_properties", (VPtr,), this=False),  # (EFI_TIMESTAMP_PROPERTIES *)
    ],
)
