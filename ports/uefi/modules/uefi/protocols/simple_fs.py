# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

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
