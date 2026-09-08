# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# EFI_BLOCK_IO_PROTOCOL — block device access. The geometry lives in a separate
# EFI_BLOCK_IO_MEDIA struct reached via the `media` pointer; overlay it with
#     media = bio.struct_at("media", BLOCK_IO_MEDIA)

from ..protocol import ProtocolDescriptor, Field, Method, Bool, U32, U64, VPtr
from .. import guid

# EFI_BLOCK_IO_MEDIA (revision-1 prefix — enough for geometry/state).
BLOCK_IO_MEDIA = ProtocolDescriptor(
    "EFI_BLOCK_IO_MEDIA",
    None,
    [
        Field("media_id", U32),
        Field("removable_media", Bool),
        Field("media_present", Bool),
        Field("logical_partition", Bool),
        Field("read_only", Bool),
        Field("write_caching", Bool),
        Field("block_size", U32),  # naturally aligned to offset 12
        Field("io_align", U32),
        Field("last_block", U64),  # naturally aligned to offset 24
    ],
)

BLOCK_IO = ProtocolDescriptor(
    "EFI_BLOCK_IO_PROTOCOL",
    guid.BLOCK_IO_PROTOCOL,
    [
        Field("revision", U64),
        Field("media", VPtr),
        Method("reset", (Bool,)),  # Reset(ExtendedVerification)
        Method("read_blocks", (U32, U64, U64, VPtr)),  # (MediaId, LBA, Size, Buffer)
        Method("write_blocks", (U32, U64, U64, VPtr)),
        Method("flush_blocks", ()),
    ],
)
