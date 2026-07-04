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

# EFI_COMPONENT_NAME2_PROTOCOL — human-readable driver / controller names.
#
# This is what the shell's `drivers` and `devtree` use to label a driver ("UEFI
# PXE Base Code Driver") or a controller. Names are returned as a CHAR16* the
# driver owns (do NOT free); read it with raw.wstr_at. Language is an RFC 4646
# tag for CN2 ("en"); the older COMPONENT_NAME uses ISO 639-2 ("eng").

from ..protocol import ProtocolDescriptor, Field, Method, Str, VPtr
from .. import guid

# GetDriverName(This, CHAR8 *Language, CHAR16 **DriverName)
# GetControllerName(This, EFI_HANDLE Controller, EFI_HANDLE Child, CHAR8 *Language, CHAR16 **Name)
COMPONENT_NAME2 = ProtocolDescriptor(
    "EFI_COMPONENT_NAME2_PROTOCOL",
    guid.COMPONENT_NAME2_PROTOCOL,
    [
        Method("get_driver_name", (Str, VPtr), check=False),
        Method("get_controller_name", (VPtr, VPtr, Str, VPtr), check=False),
        Field("supported_languages", Str),  # CHAR8* (space/semicolon-separated tags)
    ],
)

# The 1.x protocol is identical in shape; only the language encoding differs.
COMPONENT_NAME = ProtocolDescriptor(
    "EFI_COMPONENT_NAME_PROTOCOL",
    guid.COMPONENT_NAME_PROTOCOL,
    [
        Method("get_driver_name", (Str, VPtr), check=False),
        Method("get_controller_name", (VPtr, VPtr, Str, VPtr), check=False),
        Field("supported_languages", Str),
    ],
)
