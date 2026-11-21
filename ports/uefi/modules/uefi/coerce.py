# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# uefi.coerce — the package's argument-coercion vocabulary.
#
# Only four kinds of value cross UEFI API boundaries: a GUID, a pointer (or
# handle), a block of bytes, and a device path. Each circulates in more than one
# representation — an object, a raw integer, packed bytes, text — and the whole
# point of this module is that *every* boundary accepts *every* reasonable
# representation, rather than each one accepting whichever the author happened to
# have handy.
#
# The other two currencies are coerced by their own types, which are the natural
# home for them: uefi.GUID accepts a GUID / 16 bytes / a string, and
# uefi.Handle accepts a Handle / an int / any .ptr-bearing object.
#
# Kept dependency-free (uctypes only) so every module can import it without
# creating a cycle.

import uctypes


def as_ptr(value):
    """A raw pointer (int) from an int, None, or any object carrying `.ptr`.

    The `.ptr` contract in reverse: wrapper objects define `__int__`, so int()
    already does the right thing — this adds the None-is-NULL convention that
    optional pointer arguments want.
    """
    if value is None:
        return 0
    return int(value)


def as_buffer(value):
    """A buffer-protocol object from anything that is conceptually a byte block.

    Accepts a bytes/bytearray/memoryview (or any other buffer, including a GUID)
    unchanged, a PoolBuffer or other `.view`-bearing object as its view, and
    anything that knows how to serialise itself via `to_bytes()` — a DevicePath
    or a LoadOption. A str is encoded UTF-8, matching `open(..., 'w').write`.
    """
    if isinstance(value, (bytes, bytearray, memoryview)):
        return value
    view = getattr(value, "view", None)
    if view is not None:
        return view
    to_bytes = getattr(value, "to_bytes", None)
    if to_bytes is not None:
        return to_bytes()
    if isinstance(value, str):
        return value.encode()
    # Anything else with the buffer protocol (a GUID, an array, a FrameBuffer...)
    # is already usable as-is; addressof() is the cheapest way to ask.
    uctypes.addressof(value)
    return value


def as_writable(value):
    """A *writable* buffer for a region firmware will store into.

    Immutable bytes are copied into a bytearray rather than handed over: firmware
    writing through the address of an interned bytes object would corrupt every
    other reference to it. The copy is returned, so callers that need the result
    must read it back from the returned object.
    """
    if isinstance(value, bytes):
        return bytearray(value)
    return as_buffer(value)


def addr_len(value):
    """(address, length) for a byte block, without copying.

    Accepts a PoolBuffer (the .ptr/.size contract) or any buffer. Used where
    firmware retains the pointer past the call, so the caller — not this helper —
    is responsible for keeping the object alive.
    """
    ptr = getattr(value, "ptr", None)
    if ptr is not None:
        return int(ptr), len(value)
    buf = as_buffer(value)
    return uctypes.addressof(buf), len(buf)
