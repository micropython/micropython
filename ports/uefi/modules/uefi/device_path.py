# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# uefi.device_path — parse, format and build EFI device paths.
#
# A device path is a packed sequence of nodes, each a 4-byte header
# (Type, SubType, Length:u16) followed by Length-4 bytes of node data, terminated
# by an End node (Type 0x7F, SubType 0xFF). DevicePath wraps that as a list of
# DevicePathNode, with to_text()/from_text() via the firmware's DevicePathToText /
# FromText protocols (a minimal built-in formatter is used if To-Text is absent).

import uctypes

from . import raw
from . import status
from . import guid
from .protocol import ProtocolDescriptor, Method, VPtr, Bool, WStr

# Node types.
HARDWARE = 0x01
ACPI = 0x02
MESSAGING = 0x03
MEDIA = 0x04
END = 0x7F
# Selected sub-types.
HW_PCI = 0x01
ACPI_ACPI = 0x01
MEDIA_HARDDRIVE = 0x01
MEDIA_FILEPATH = 0x04
END_INSTANCE = 0x01
END_ENTIRE = 0xFF

_END_NODE = bytes((END, END_ENTIRE, 0x04, 0x00))

# DevicePathToText / FromText are stateless utility protocols: their methods take
# NO `This` pointer (this=False), and return CHAR16* / EFI_DEVICE_PATH* rather than
# a status (check=False -> the stub yields the raw pointer).
DEVICE_PATH_TO_TEXT = ProtocolDescriptor(
    "EFI_DEVICE_PATH_TO_TEXT_PROTOCOL",
    guid.DEVICE_PATH_TO_TEXT_PROTOCOL,
    [
        Method("convert_device_node_to_text", (VPtr, Bool, Bool), check=False, this=False),
        Method("convert_device_path_to_text", (VPtr, Bool, Bool), check=False, this=False),
    ],
)
DEVICE_PATH_FROM_TEXT = ProtocolDescriptor(
    "EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL",
    guid.DEVICE_PATH_FROM_TEXT_PROTOCOL,
    [
        Method("convert_text_to_device_node", (WStr,), check=False, this=False),
        Method("convert_text_to_device_path", (WStr,), check=False, this=False),
    ],
)

_to_text = None  # cached wrapper, or False if the protocol is absent
_from_text = None


def _totext():
    global _to_text
    if _to_text is None:
        try:
            _to_text = DEVICE_PATH_TO_TEXT.locate()
        except status.Error:
            _to_text = False
    return _to_text or None


def _fromtext():
    global _from_text
    if _from_text is None:
        _from_text = DEVICE_PATH_FROM_TEXT.locate()  # raises uefi.Error if absent
    return _from_text


def _utf16le_decode(b):
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


class DevicePathNode:
    def __init__(self, type, subtype, data=b""):
        self.type = type
        self.subtype = subtype
        self.data = bytes(data)

    def to_bytes(self):
        length = 4 + len(self.data)
        return bytes((self.type, self.subtype, length & 0xFF, (length >> 8) & 0xFF)) + self.data

    def __len__(self):
        return 4 + len(self.data)

    def __repr__(self):
        return "DevicePathNode(0x%02x, 0x%02x, %d bytes)" % (
            self.type,
            self.subtype,
            len(self.data),
        )


class DevicePath:
    def __init__(self, nodes=None):
        self.nodes = list(nodes) if nodes else []

    @classmethod
    def from_ptr(cls, ptr):
        """Parse a device path from a raw EFI_DEVICE_PATH_PROTOCOL pointer."""
        nodes = []
        ptr = int(ptr)
        while True:
            hdr = uctypes.bytes_at(ptr, 4)
            length = hdr[2] | (hdr[3] << 8)
            if length < 4 or hdr[0] == END:
                break
            data = uctypes.bytes_at(ptr + 4, length - 4) if length > 4 else b""
            nodes.append(DevicePathNode(hdr[0], hdr[1], data))
            ptr += length
        return cls(nodes)

    @classmethod
    def from_bytes(cls, b):
        nodes = []
        i = 0
        while i + 4 <= len(b):
            length = b[i + 2] | (b[i + 3] << 8)
            if length < 4 or i + length > len(b) or b[i] == END:
                break
            nodes.append(DevicePathNode(b[i], b[i + 1], bytes(b[i + 4 : i + length])))
            i += length
        return cls(nodes)

    @classmethod
    def from_text(cls, text):
        """Parse text (e.g. 'PciRoot(0x0)/Pci(0x1,0x0)') via DevicePathFromText."""
        p = _fromtext().convert_text_to_device_path(text)
        if not p:
            raise ValueError("could not parse device path text")
        dp = cls.from_ptr(p)
        raw.free_pool(p)
        return dp

    def to_bytes(self):
        return b"".join(n.to_bytes() for n in self.nodes) + _END_NODE

    def to_text(self, display_only=False, allow_shortcuts=False):
        proto = _totext()
        if proto is not None:
            packed = self.to_bytes()
            p = proto.convert_device_path_to_text(
                uctypes.addressof(packed), display_only, allow_shortcuts
            )
            if p:
                s = raw.wstr_at(p)
                raw.free_pool(p)
                return s
        return "/".join("%02x/%02x" % (n.type, n.subtype) for n in self.nodes)

    # ── typed node accessors ────────────────────────────────────────────────
    def file_path(self):
        """The path string from the first Media/FilePath node, or None."""
        for n in self.nodes:
            if n.type == MEDIA and n.subtype == MEDIA_FILEPATH:
                return _utf16le_decode(n.data)
        return None

    def pci_node(self):
        """(device, function) from the first Hardware/PCI node, or None."""
        for n in self.nodes:
            if n.type == HARDWARE and n.subtype == HW_PCI and len(n.data) >= 2:
                return (n.data[1], n.data[0])  # data is [Function, Device]
        return None

    def acpi_node(self):
        """(HID, UID) from the first ACPI node, or None."""
        for n in self.nodes:
            if n.type == ACPI and n.subtype == ACPI_ACPI and len(n.data) >= 8:
                hid = n.data[0] | (n.data[1] << 8) | (n.data[2] << 16) | (n.data[3] << 24)
                uid = n.data[4] | (n.data[5] << 8) | (n.data[6] << 16) | (n.data[7] << 24)
                return (hid, uid)
        return None

    def __iter__(self):
        return iter(self.nodes)

    def __len__(self):
        return len(self.nodes)

    def __repr__(self):
        return "DevicePath(%r)" % self.to_text()
