# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

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
