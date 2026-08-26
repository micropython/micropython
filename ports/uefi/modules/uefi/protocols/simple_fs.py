# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# EFI_SIMPLE_FILE_SYSTEM_PROTOCOL — opens the root directory of a FAT volume.
# open_volume() returns the root EFI_FILE_PROTOCOL* (an Out slot, so the stub
# owns the scratch buffer).

from ..protocol import ProtocolDescriptor, Field, Method, Out, U64, VPtr
from .. import guid

SIMPLE_FS = ProtocolDescriptor(
    "EFI_SIMPLE_FILE_SYSTEM_PROTOCOL",
    guid.SIMPLE_FILE_SYSTEM_PROTOCOL,
    [
        Field("revision", U64),
        Method("open_volume", (Out(VPtr),)),  # OpenVolume(**Root) -> the root ptr
    ],
)
