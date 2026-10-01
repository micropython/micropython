# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# uefi.raw — the low-level UEFI surface.
#
# A thin re-export of the `_uefi` C built-in: Boot/Runtime Services primitives
# plus the EFIAPI `call` trampoline. Every function returns a raw status int (or a
# tuple including one) and never raises — call uefi.status.check() for exception
# semantics. See README.md for the full surface.

from _uefi import *  # noqa: F401,F403
