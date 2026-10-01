# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# uefi.status — EFI_STATUS constants and the uefi.Error exception.
#
# EFI_STATUS is a UINTN whose top bit marks an error. The pythonic layer calls
# check() on raw return values to turn the error range into a raised uefi.Error
# (a subclass of OSError, so existing `except OSError` paths still catch it).
#
# The values below are genuinely 64-bit: EFI_STATUS is spec'd as a native-word-size
# UINTN, both architectures this port targets are 64-bit, and raw firmware calls
# return exactly these bit patterns — the same width is required so a raw status
# can be compared directly against these constants (e.g. `st == status.NOT_FOUND`)
# with no masking. Each is spelled as _HIGH_BIT | <small error code> so that
# structure (one flag bit + a small code) is visible in the source, rather than
# 16 hex digits that only differ in the last one or two.

from micropython import const

_HIGH_BIT = const(1 << 63)

SUCCESS = 0

# Error codes (top bit set). Small-code values per the UEFI spec, Appendix D.
LOAD_ERROR = _HIGH_BIT | 1
INVALID_PARAMETER = _HIGH_BIT | 2
UNSUPPORTED = _HIGH_BIT | 3
BAD_BUFFER_SIZE = _HIGH_BIT | 4
BUFFER_TOO_SMALL = _HIGH_BIT | 5
NOT_READY = _HIGH_BIT | 6
DEVICE_ERROR = _HIGH_BIT | 7
WRITE_PROTECTED = _HIGH_BIT | 8
OUT_OF_RESOURCES = _HIGH_BIT | 9
VOLUME_CORRUPTED = _HIGH_BIT | 0xA
VOLUME_FULL = _HIGH_BIT | 0xB
NO_MEDIA = _HIGH_BIT | 0xC
MEDIA_CHANGED = _HIGH_BIT | 0xD
NOT_FOUND = _HIGH_BIT | 0xE
ACCESS_DENIED = _HIGH_BIT | 0xF
NO_RESPONSE = _HIGH_BIT | 0x10
NO_MAPPING = _HIGH_BIT | 0x11
TIMEOUT = _HIGH_BIT | 0x12
NOT_STARTED = _HIGH_BIT | 0x13
ALREADY_STARTED = _HIGH_BIT | 0x14
ABORTED = _HIGH_BIT | 0x15
ICMP_ERROR = _HIGH_BIT | 0x16
TFTP_ERROR = _HIGH_BIT | 0x17
PROTOCOL_ERROR = _HIGH_BIT | 0x18
INCOMPATIBLE_VERSION = _HIGH_BIT | 0x19
SECURITY_VIOLATION = _HIGH_BIT | 0x1A
CRC_ERROR = _HIGH_BIT | 0x1B
END_OF_MEDIA = _HIGH_BIT | 0x1C
END_OF_FILE = _HIGH_BIT | 0x1F
INVALID_LANGUAGE = _HIGH_BIT | 0x20
COMPROMISED_DATA = _HIGH_BIT | 0x21

_NAMES = {
    SUCCESS: "EFI_SUCCESS",
    LOAD_ERROR: "EFI_LOAD_ERROR",
    INVALID_PARAMETER: "EFI_INVALID_PARAMETER",
    UNSUPPORTED: "EFI_UNSUPPORTED",
    BAD_BUFFER_SIZE: "EFI_BAD_BUFFER_SIZE",
    BUFFER_TOO_SMALL: "EFI_BUFFER_TOO_SMALL",
    NOT_READY: "EFI_NOT_READY",
    DEVICE_ERROR: "EFI_DEVICE_ERROR",
    WRITE_PROTECTED: "EFI_WRITE_PROTECTED",
    OUT_OF_RESOURCES: "EFI_OUT_OF_RESOURCES",
    VOLUME_CORRUPTED: "EFI_VOLUME_CORRUPTED",
    VOLUME_FULL: "EFI_VOLUME_FULL",
    NO_MEDIA: "EFI_NO_MEDIA",
    MEDIA_CHANGED: "EFI_MEDIA_CHANGED",
    NOT_FOUND: "EFI_NOT_FOUND",
    ACCESS_DENIED: "EFI_ACCESS_DENIED",
    NO_RESPONSE: "EFI_NO_RESPONSE",
    NO_MAPPING: "EFI_NO_MAPPING",
    TIMEOUT: "EFI_TIMEOUT",
    NOT_STARTED: "EFI_NOT_STARTED",
    ALREADY_STARTED: "EFI_ALREADY_STARTED",
    ABORTED: "EFI_ABORTED",
    ICMP_ERROR: "EFI_ICMP_ERROR",
    TFTP_ERROR: "EFI_TFTP_ERROR",
    PROTOCOL_ERROR: "EFI_PROTOCOL_ERROR",
    INCOMPATIBLE_VERSION: "EFI_INCOMPATIBLE_VERSION",
    SECURITY_VIOLATION: "EFI_SECURITY_VIOLATION",
    CRC_ERROR: "EFI_CRC_ERROR",
    END_OF_MEDIA: "EFI_END_OF_MEDIA",
    END_OF_FILE: "EFI_END_OF_FILE",
    INVALID_LANGUAGE: "EFI_INVALID_LANGUAGE",
    COMPROMISED_DATA: "EFI_COMPROMISED_DATA",
}


def name(status):
    """Human-readable EFI_STATUS name, e.g. 'EFI_NOT_FOUND'."""
    n = _NAMES.get(status)
    if n is not None:
        return n
    return "EFI_STATUS_0x%016X" % status


class Error(OSError):
    """Raised when a UEFI call returns a non-SUCCESS EFI_STATUS."""

    def __init__(self, status):
        self.status = status
        self.name = name(status)
        super().__init__(status, self.name)


def check(status):
    """Raise uefi.Error if status is in the error range; return status otherwise."""
    if status & _HIGH_BIT:
        raise Error(status)
    return status
