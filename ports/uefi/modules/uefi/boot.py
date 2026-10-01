# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# uefi.boot — the UEFI boot-manager layer: read/parse/write the standard boot
# variables (BootOrder, Boot####, BootNext) and launch a boot option.
#
# This is the substance of a Python boot loader / boot manager. A boot option is
# an EFI_LOAD_OPTION stored in the Boot#### global variable; BootOrder lists the
# option numbers in priority order. To boot one we LoadImage its device path,
# hand its OptionalData to the image as LoadOptions, and StartImage it — exactly
# what the firmware's own boot manager does (and what `bcfg`/`boot` drive).
#
# Chainloading an OS this way needs no ExitBootServices from us: the OS loader we
# start (shim/GRUB/systemd-boot/bootmgfw/an EFI-stub kernel) calls it itself.

from . import raw
from . import status
from . import variable
from . import guid as _guid
from . import utf16
from .device_path import DevicePath
from .driver import Image

# Boot variables live in the EFI global-variable namespace.
_NS = _guid.GLOBAL_VARIABLE

# LOAD_OPTION_* attribute flags (EFI_LOAD_OPTION.Attributes).
ACTIVE = 0x00000001
FORCE_RECONNECT = 0x00000002
HIDDEN = 0x00000008
CATEGORY_APP = 0x00000100

# Boot variables should persist and be visible at runtime.
_VAR_ATTRS = variable.NON_VOLATILE | variable.BOOTSERVICE_ACCESS | variable.RUNTIME_ACCESS


class LoadOption:
    """An EFI_LOAD_OPTION: an OS/app to boot (description + device path + data)."""

    def __init__(self, description, device_path, optional_data=b"", attributes=ACTIVE):
        self.description = description
        self.device_path = device_path  # a DevicePath
        self.optional_data = bytes(optional_data)
        self.attributes = attributes

    @property
    def active(self):
        return bool(self.attributes & ACTIVE)

    @classmethod
    def parse(cls, data):
        """Parse the packed EFI_LOAD_OPTION bytes of a Boot#### variable."""
        attributes = int.from_bytes(data[0:4], "little")
        fpl = int.from_bytes(data[4:6], "little")  # FilePathListLength (bytes)
        # Description: NUL-terminated CHAR16 starting at offset 6.
        end = 6
        while end + 1 < len(data) and not (data[end] == 0 and data[end + 1] == 0):
            end += 2
        description = utf16.decode(data[6:end])
        dp_start = end + 2  # skip the CHAR16 NUL
        dp_bytes = data[dp_start : dp_start + fpl]
        device_path = DevicePath.from_bytes(dp_bytes)
        optional_data = bytes(data[dp_start + fpl :])
        return cls(description, device_path, optional_data, attributes)

    def to_bytes(self):
        """Serialise to the packed EFI_LOAD_OPTION form for SetVariable."""
        dp = self.device_path.to_bytes()
        desc = utf16.encode(self.description)
        return (
            self.attributes.to_bytes(4, "little")
            + len(dp).to_bytes(2, "little")
            + desc
            + dp
            + self.optional_data
        )

    def load(self, *, boot_policy=True):
        """LoadImage this option's device path; return an unstarted Image."""
        img = Image.load_from_device_path(self.device_path, boot_policy=boot_policy)
        if self.optional_data:
            img.set_load_options(self.optional_data)
        return img

    def boot(self, *, boot_policy=True):
        """Load and StartImage this option; return (exit_status, exit_data)."""
        return self.load(boot_policy=boot_policy).start()

    def __repr__(self):
        return "LoadOption(%r, %r, active=%s)" % (
            self.description,
            self.device_path.to_text(),
            self.active,
        )


def _var_name(num):
    return "Boot%04X" % (num & 0xFFFF)


def boot_order():
    """The BootOrder list — boot-option numbers in priority order ([] if unset)."""
    if not variable.exists("BootOrder", _NS):
        return []
    data = variable.get("BootOrder", _NS)
    return [int.from_bytes(data[i : i + 2], "little") for i in range(0, len(data), 2)]


def set_boot_order(nums):
    """Write BootOrder from a sequence of option numbers."""
    data = b"".join((n & 0xFFFF).to_bytes(2, "little") for n in nums)
    variable.set("BootOrder", _NS, data, _VAR_ATTRS)


def load_option(num):
    """Parse Boot#### into a LoadOption (raises uefi.Error if it doesn't exist)."""
    return LoadOption.parse(variable.get(_var_name(num), _NS))


def set_option(num, option):
    """Write a LoadOption to Boot####."""
    variable.set(_var_name(num), _NS, option.to_bytes(), _VAR_ATTRS)


def options():
    """List (num, LoadOption) for every entry in BootOrder (skipping unreadable ones)."""
    out = []
    for n in boot_order():
        try:
            out.append((n, load_option(n)))
        except status.Error:
            pass
    return out


def set_boot_next(num):
    """Set BootNext so the firmware boots option `num` on the next reset."""
    variable.set("BootNext", _NS, (num & 0xFFFF).to_bytes(2, "little"), _VAR_ATTRS)


def boot(num, *, boot_policy=True):
    """Load and start boot option `num`; return (exit_status, exit_data)."""
    return load_option(num).boot(boot_policy=boot_policy)
