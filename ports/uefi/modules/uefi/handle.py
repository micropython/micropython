# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# uefi.handle — Handle wrappers and protocol open/close ownership.
#
# Handle wraps a raw EFI_HANDLE (exposed as `.ptr`, per the .ptr contract) and is
# the entry point for the handle database: enumerate the protocols on a handle,
# test for a protocol, or open one. A ProtocolHandle owns an OpenProtocol/
# CloseProtocol pair (or a bare LocateProtocol result), so the firmware open is
# released deterministically via close()/context-manager/__del__.

from micropython import const

from . import raw
from . import status
from .guid import GUID

# EFI_OPEN_PROTOCOL_* attributes (EDK2 prefix stripped per naming convention).
BY_HANDLE_PROTOCOL = 0x01
GET_PROTOCOL = 0x02
TEST_PROTOCOL = 0x04
BY_CHILD_CONTROLLER = 0x08
BY_DRIVER = 0x10
EXCLUSIVE = 0x20

# EFI_LOCATE_SEARCH_TYPE values (for raw.locate_handle_buffer).
_ALL_HANDLES = const(0)
_BY_PROTOCOL = const(2)


class Handle:
    def __init__(self, ptr):
        self.ptr = int(ptr)

    def __eq__(self, other):
        return isinstance(other, Handle) and other.ptr == self.ptr

    def __hash__(self):
        return hash(self.ptr)

    def __repr__(self):
        return "Handle(0x%x)" % self.ptr

    def protocols(self):
        st, lst = raw.protocols_per_handle(self.ptr)
        status.check(st)
        return [GUID(g) for g in lst]

    def supports(self, guid):
        want = GUID(guid).bytes
        st, lst = raw.protocols_per_handle(self.ptr)
        status.check(st)
        return any(g == want for g in lst)

    def open(self, guid, attributes=GET_PROTOCOL):
        return ProtocolHandle.open(self, guid, attributes)

    def device_path(self):
        """This handle's DevicePath, or None if it has no Device Path protocol."""
        from .guid import DEVICE_PATH_PROTOCOL
        from . import device_path as _dp

        st, iface = raw.open_protocol(
            self.ptr, DEVICE_PATH_PROTOCOL.bytes, raw.image_handle(), 0, GET_PROTOCOL
        )
        if st == status.NOT_FOUND:
            return None
        status.check(st)
        return _dp.DevicePath.from_ptr(iface)

    def open_info(self, guid):
        """Who has `guid` open on this handle, and how — a list of ProtocolOpenInfo.

        This is the usage view behind the shell's `dh -d` / `devices` / `devtree`:
        each entry's `by_driver` / `by_child` tells you the driver↔controller
        relationships. Empty list if the protocol is present but unopened.
        """
        st, entries = raw.open_protocol_information(self.ptr, GUID(guid).bytes)
        if st == status.NOT_FOUND:
            return []
        status.check(st)
        return [ProtocolOpenInfo(*e) for e in entries]


class ProtocolOpenInfo:
    # One EFI_OPEN_PROTOCOL_INFORMATION_ENTRY: an agent's open of a protocol on a
    # handle. `agent` is the driver/app image handle; `controller` the controller
    # handle (for driver/child opens); the attribute flags say how it was opened.
    def __init__(self, agent, controller, attributes, open_count):
        self.agent = Handle(agent) if agent else None
        self.controller = Handle(controller) if controller else None
        self.attributes = attributes
        self.open_count = open_count
        self.by_driver = bool(attributes & BY_DRIVER)
        self.by_child = bool(attributes & BY_CHILD_CONTROLLER)
        self.exclusive = bool(attributes & EXCLUSIVE)

    def __repr__(self):
        kind = "DRIVER" if self.by_driver else "CHILD" if self.by_child else "GET"
        return "ProtocolOpenInfo(agent=%r, controller=%r, %s, count=%d)" % (
            self.agent,
            self.controller,
            kind,
            self.open_count,
        )


class ProtocolHandle:
    # Owns a firmware protocol open. `.ptr` is the interface pointer (the .ptr
    # contract) — ready to hand to raw.call for method dispatch.
    def __init__(self, ptr, handle=None, guid=None, attributes=0, owns=False):
        self.ptr = int(ptr)
        self._handle = handle
        self._guid = guid
        self._attrs = attributes
        self._owns = owns

    @classmethod
    def open(cls, handle, guid, attributes=GET_PROTOCOL):
        g = GUID(guid)
        agent = raw.image_handle()
        st, iface = raw.open_protocol(handle.ptr, g.bytes, agent, 0, attributes)
        status.check(st)
        return cls(iface, handle, g, attributes, owns=True)

    @classmethod
    def locate(cls, guid):
        g = GUID(guid)
        st, iface = raw.locate_protocol(g.bytes)
        status.check(st)
        return cls(iface, None, g, 0, owns=False)

    def close(self):
        # Only a tracked open (OpenProtocol) needs CloseProtocol; GET_PROTOCOL
        # opens aren't tracked by firmware, so its NOT_FOUND return is ignored.
        if self._owns and self.ptr and self._handle is not None:
            raw.close_protocol(self._handle.ptr, self._guid.bytes, raw.image_handle(), 0)
        self._owns = False
        self.ptr = 0

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.close()

    def __del__(self):
        self.close()

    def __repr__(self):
        return "ProtocolHandle(ptr=0x%x, guid=%s)" % (self.ptr, self._guid)


# ── Discovery helpers ────────────────────────────────────────────────────────
def all_handles():
    st, lst = raw.locate_handle_buffer(_ALL_HANDLES, None)
    status.check(st)
    return [Handle(h) for h in lst]


def locate_handles(guid):
    st, lst = raw.locate_handle_buffer(_BY_PROTOCOL, GUID(guid).bytes)
    status.check(st)
    return [Handle(h) for h in lst]


def locate_protocol(guid):
    # Returns an (unowned) ProtocolHandle around the firmware's arbitrary instance
    # of `guid`. Use locate_handles() when several instances may exist.
    return ProtocolHandle.locate(guid)


def all_protocols():
    """The set of every protocol GUID installed anywhere in the handle database.

    The union of each handle's protocols — the global protocol inventory the shell
    shows. Returns a set of GUID (hashable), so `len(all_protocols())` is the
    distinct-protocol count.
    """
    seen = set()
    for h in all_handles():
        for g in h.protocols():
            seen.add(g)
    return seen
