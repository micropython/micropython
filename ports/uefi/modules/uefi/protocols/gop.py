# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

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
