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

# EFI_LOADED_IMAGE_PROTOCOL — describes a loaded image (driver or application).
# Mostly a data protocol; `unload` is the only method (and may be NULL).

from ..protocol import ProtocolDescriptor, Field, Method, U32, U64, VPtr
from .. import guid

LOADED_IMAGE = ProtocolDescriptor(
    "EFI_LOADED_IMAGE_PROTOCOL",
    guid.LOADED_IMAGE_PROTOCOL,
    [
        Field("revision", U32),
        Field("parent_handle", VPtr),
        Field("system_table", VPtr),
        Field("device_handle", VPtr),  # the volume this image was loaded from
        Field("file_path", VPtr),
        Field("reserved", VPtr),
        Field("load_options_size", U32),
        Field("load_options", VPtr),
        Field("image_base", VPtr),
        Field("image_size", U64),
        Field("image_code_type", U32),
        Field("image_data_type", U32),
        Method("unload", (VPtr,)),  # Unload(ImageHandle)
    ],
)
