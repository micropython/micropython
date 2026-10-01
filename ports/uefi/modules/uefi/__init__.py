# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# uefi — Pythonic access to the UEFI environment.
#
# Two layers (see README.md):
#   uefi.raw  — the low-level C surface (Boot/Runtime Services primitives + the
#               EFIAPI `call` trampoline). Returns raw status ints; never raises.
#   uefi.*    — resource-safe, Pythonic wrappers built on uefi.raw.
#
# The C half is the built-in module `_uefi`; uefi.raw re-exports it.

from . import raw
from . import status
from . import guid
from . import utf16
from . import buffer
from . import handle
from . import protocol
from . import protocols
from . import variable
from . import event
from . import device_path
from . import driver
from . import system
from . import boot
from . import load_file
from . import fs
from . import console
from . import display

from _uefi import Event
from .status import Error, check
from .guid import GUID
from .buffer import PoolBuffer
from .handle import Handle, ProtocolHandle
from .protocol import ProtocolDescriptor, ProtocolWrapper
from .variable import VariablePolicy
from .event import Timer, ProtocolWatch
from .device_path import DevicePath, DevicePathNode
from .driver import Image
from .boot import LoadOption
from .load_file import FileProvider
from .console import Console
from .display import Display
