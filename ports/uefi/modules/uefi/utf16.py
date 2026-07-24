# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# uefi.utf16 — CHAR16 (UTF-16LE) encode/decode, shared by every module that talks
# to firmware wide-string fields (boot options, variable names, device-path text,
# LoadOptions, ...). One copy: encode() was previously duplicated verbatim across
# boot.py/protocol.py/variable.py/driver.py, and decode() existed in two places
# with different behaviour (a naive UCS-2-only version alongside a correct,
# surrogate-pair-aware one) — this file keeps the correct decoder only.


def encode(s):
    """Encode a str to NUL-terminated UTF-16LE bytes (a CHAR16* string)."""
    b = bytearray()
    for ch in s:
        c = ord(ch)
        b.append(c & 0xFF)
        b.append((c >> 8) & 0xFF)
    b.append(0)
    b.append(0)
    return bytes(b)


def decode(b):
    """Decode NUL-terminated (or whole-buffer) UTF-16LE bytes to a str.

    Handles surrogate pairs, so codepoints outside the BMP round-trip correctly.
    """
    out = []
    i = 0
    n = len(b)
    while i + 1 < n:
        c = b[i] | (b[i + 1] << 8)
        i += 2
        if c == 0:
            break
        if 0xD800 <= c <= 0xDBFF and i + 1 < n:
            lo = b[i] | (b[i + 1] << 8)
            if 0xDC00 <= lo <= 0xDFFF:
                i += 2
                c = 0x10000 + ((c - 0xD800) << 10) + (lo - 0xDC00)
        out.append(chr(c))
    return "".join(out)
