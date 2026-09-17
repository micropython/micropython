# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# uefi.system — platform-wide services: reset, watchdog, memory map, firmware
# configuration tables (ACPI/SMBIOS/DTB), and System Table identity.
#
# These are the non-device, whole-machine operations an EFI-shell user reaches
# for: `reset`, `memmap`, and the data behind `acpiview` / `smbiosview` (the
# configuration-table pointers; parsing the tables themselves is then pure Python).

import uctypes

from . import raw
from . import status
from . import guid as _guid
from .guid import GUID

# EFI_RESET_TYPE values.
COLD = 0
WARM = 1
SHUTDOWN = 2
PLATFORM_SPECIFIC = 3

# EFI_TIME.TimeZone sentinel: store as local time, zone unknown.
UNSPECIFIED_TIMEZONE = 0x07FF


def reset(mode=COLD, status_code=0, data=None):
    """Reset the platform (ResetSystem). Does NOT return for COLD/WARM/SHUTDOWN.

    `data` is optional reset payload bytes (e.g. a NUL-terminated string for
    PLATFORM_SPECIFIC resets). Note: this is a deliberate platform reset — the
    port's *own* clean exit returns EFI_SUCCESS to its caller instead.
    """
    raw.reset_system(mode, status_code, data)


def shutdown():
    """Power the platform off (ResetSystem EfiResetShutdown). Does not return."""
    raw.reset_system(SHUTDOWN, 0, None)


def set_watchdog(seconds, code=0):
    """Arm the boot watchdog for `seconds` (0 disables it — do this before a handoff)."""
    status.check(raw.set_watchdog_timer(int(seconds), code))


class MemoryDescriptor:
    """One EFI_MEMORY_DESCRIPTOR: a run of `pages` 4 KiB pages of a given type."""

    PAGE_SIZE = 4096

    def __init__(self, type, physical_start, num_pages, attribute):
        self.type = type
        self.physical_start = physical_start
        self.num_pages = num_pages
        self.attribute = attribute

    @property
    def size(self):
        return self.num_pages * self.PAGE_SIZE

    @property
    def physical_end(self):
        return self.physical_start + self.size

    def __repr__(self):
        return "MemoryDescriptor(type=%d, 0x%x..0x%x, %d pages)" % (
            self.type,
            self.physical_start,
            self.physical_end,
            self.num_pages,
        )


def memory_map():
    """Return (map_key, [MemoryDescriptor, ...]) — the current UEFI memory map."""
    st, key, entries = raw.get_memory_map()
    status.check(st)
    return key, [MemoryDescriptor(*e) for e in entries]


def configuration_tables():
    """List (GUID, table_ptr) for every System Table configuration table."""
    return [(GUID(g), ptr) for (g, ptr) in raw.configuration_tables()]


def configuration_table(which):
    """The pointer for one configuration table by GUID, or 0 if not present."""
    want = GUID(which)
    for g, ptr in raw.configuration_tables():
        if GUID(g) == want:
            return ptr
    return 0


def acpi_rsdp():
    """Pointer to the ACPI 2.0+ RSDP (falls back to ACPI 1.0), or 0 if absent."""
    return configuration_table(_guid.ACPI_20_TABLE_GUID) or configuration_table(
        _guid.ACPI_TABLE_GUID
    )


def smbios():
    """Pointer to the SMBIOS 3 entry point (falls back to SMBIOS 2.x), or 0."""
    return configuration_table(_guid.SMBIOS3_TABLE_GUID) or configuration_table(
        _guid.SMBIOS_TABLE_GUID
    )


def device_tree():
    """Pointer to the flattened device tree (DTB) blob, or 0 if absent."""
    return configuration_table(_guid.DTB_TABLE_GUID)


# ── Real-time clock (Runtime Services) + monotonic count ─────────────────────
class Time:
    """A wall-clock reading (EFI_TIME). `timezone` is signed minutes from UTC, or
    UNSPECIFIED_TIMEZONE for local time; `daylight` is a bitfield."""

    def __init__(
        self,
        year,
        month,
        day,
        hour,
        minute,
        second,
        nanosecond=0,
        timezone=UNSPECIFIED_TIMEZONE,
        daylight=0,
    ):
        self.year = year
        self.month = month
        self.day = day
        self.hour = hour
        self.minute = minute
        self.second = second
        self.nanosecond = nanosecond
        self.timezone = timezone
        self.daylight = daylight

    def __repr__(self):
        return "%04d-%02d-%02d %02d:%02d:%02d" % (
            self.year,
            self.month,
            self.day,
            self.hour,
            self.minute,
            self.second,
        )


def get_time():
    """Read the platform real-time clock as a Time."""
    st, t = raw.get_time()
    status.check(st)
    return Time(*t)


def set_time(
    year, month, day, hour, minute, second, nanosecond=0, timezone=UNSPECIFIED_TIMEZONE, daylight=0
):
    """Set the platform real-time clock (the shell's `time`/`date`)."""
    status.check(
        raw.set_time(year, month, day, hour, minute, second, nanosecond, timezone, daylight)
    )


def monotonic_count():
    """The next value of the platform monotonic counter (the shell's `getmtc`)."""
    st, count = raw.get_next_monotonic_count()
    status.check(st)
    return count


# ── System Table identity ────────────────────────────────────────────────────
# EFI_SYSTEM_TABLE: 24-byte header, then FirmwareVendor (CHAR16*) @24 and
# FirmwareRevision (UINT32) @32 on both 64-bit targets.
_ST_LAYOUT = {
    "firmware_vendor": uctypes.UINT64 | 24,
    "firmware_revision": uctypes.UINT32 | 32,
}


def firmware_vendor():
    """The firmware vendor string (e.g. 'EDK II')."""
    st = uctypes.struct(raw.system_table_ptr(), _ST_LAYOUT, uctypes.LITTLE_ENDIAN)
    return raw.wstr_at(st.firmware_vendor)


def firmware_revision():
    """The firmware revision (a vendor-defined UINT32)."""
    st = uctypes.struct(raw.system_table_ptr(), _ST_LAYOUT, uctypes.LITTLE_ENDIAN)
    return st.firmware_revision
