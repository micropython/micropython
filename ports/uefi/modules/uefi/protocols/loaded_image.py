# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

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
