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
