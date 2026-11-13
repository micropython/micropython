# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# uefi — Pythonic access to the UEFI environment.
#
# Two layers (see README.md):
#   uefi.raw  — the low-level C surface (Boot/Runtime Services primitives + the
#               EFIAPI `call` trampoline). Returns raw status ints; never raises.
#   uefi.*    — resource-safe, Pythonic wrappers built on uefi.raw. Raise
#               uefi.Error on a firmware error; release what they own through
#               close(), a context manager, or finalisation.
#
# The C half is the built-in module `_uefi`; uefi.raw re-exports it.
#
# Values compose across both layers: whatever one call returns is accepted by the
# calls that consume it, with no unpacking in between. Four kinds of value cross
# API boundaries, and each is taken in any of its spellings —
#   GUID          a uefi.GUID (itself a 16-byte buffer), 16 bytes, or a string
#   pointer       a bare int, or any object exposing `.ptr` (Handle, Image,
#                 ProtocolHandle, ProtocolWrapper, PoolBuffer, Event, Timer, ...)
#   byte block    bytes/bytearray/memoryview, a PoolBuffer, a DevicePath, or
#                 anything with to_bytes()
#   device path   a DevicePath, its packed bytes, its text, or a firmware pointer
# uefi.GUID(), uefi.Handle(), uefi.as_device_path() and uefi.coerce are the
# coercions themselves, for code that needs to normalise a value directly.

from . import raw
from . import status
from . import coerce
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

from _uefi import Event, GUID
from .status import Error, check
from .buffer import PoolBuffer
from .handle import Handle, ProtocolHandle
from .protocol import ProtocolDescriptor, ProtocolWrapper
from .variable import VariablePolicy
from .event import Timer, ProtocolWatch
from .device_path import DevicePath, DevicePathNode, as_device_path
from .driver import Image
from .boot import LoadOption
from .load_file import FileProvider
from .console import Console
from .display import Display
