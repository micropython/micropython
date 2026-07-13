# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

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
