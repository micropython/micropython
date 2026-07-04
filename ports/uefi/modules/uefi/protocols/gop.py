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

# EFI_GRAPHICS_OUTPUT_PROTOCOL (GOP) — framebuffer + mode query/set/blt.
# Current-mode geometry is reached through the `mode` pointer, then its `info`:
#     mode = gop.struct_at("mode", GOP_MODE)
#     info = mode.struct_at("info", GOP_INFO)
#     w, h = info.horizontal_resolution, info.vertical_resolution

from ..protocol import ProtocolDescriptor, Field, Method, U32, U64, VPtr
from .. import guid

# EFI_GRAPHICS_OUTPUT_MODE_INFORMATION.
GOP_INFO = ProtocolDescriptor(
    "EFI_GRAPHICS_OUTPUT_MODE_INFORMATION",
    None,
    [
        Field("version", U32),
        Field("horizontal_resolution", U32),
        Field("vertical_resolution", U32),
        Field("pixel_format", U32),
        Field("pixel_red_mask", U32),
        Field("pixel_green_mask", U32),
        Field("pixel_blue_mask", U32),
        Field("pixel_reserved_mask", U32),
        Field("pixels_per_scan_line", U32),
    ],
)

# EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE.
GOP_MODE = ProtocolDescriptor(
    "EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE",
    None,
    [
        Field("max_mode", U32),
        Field("mode", U32),
        Field("info", VPtr),
        Field("size_of_info", U64),
        Field("frame_buffer_base", U64),
        Field("frame_buffer_size", U64),
    ],
)

GOP = ProtocolDescriptor(
    "EFI_GRAPHICS_OUTPUT_PROTOCOL",
    guid.GRAPHICS_OUTPUT_PROTOCOL,
    [
        Method("query_mode", (U32, VPtr, VPtr)),  # (ModeNumber, *SizeOfInfo, **Info)
        Method("set_mode", (U32,)),
        # Blt(*BltBuffer, BltOperation, SourceX, SourceY, DestX, DestY, Width, Height, Delta)
        Method("blt", (VPtr, U32, U64, U64, U64, U64, U64, U64, U64)),
        Field("mode", VPtr),
    ],
)
