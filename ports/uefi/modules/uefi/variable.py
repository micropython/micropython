# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# uefi.variable — UEFI NVRAM variables (Runtime Services) + Variable Policy.
#
# Read/write/enumerate firmware variables, and (where the firmware provides it)
# register Variable Policy constraints. Names are Python str (marshalled to CHAR16
# by the raw layer); namespaces are GUIDs; data is bytes.
#
# Pitfalls (see README.md): SetVariable on non-volatile vars
# writes flash — don't loop. Enumeration order isn't stable; collect fully before
# mutating. RUNTIME_ACCESS without NON_VOLATILE is a volatile runtime variable.

from micropython import const

from . import raw
from . import status
from .guid import GUID
from .protocol import ProtocolDescriptor, Field, Method, U64, VPtr
from . import guid as _guid
from . import utf16

# EFI_VARIABLE_* attribute flags (EFI_VARIABLE_ prefix stripped per convention).
NON_VOLATILE = 0x01
BOOTSERVICE_ACCESS = 0x02
RUNTIME_ACCESS = 0x04
HARDWARE_ERROR_RECORD = 0x08
AUTHENTICATED_WRITE_ACCESS = 0x10  # deprecated
TIME_BASED_AUTHENTICATED_WRITE_ACCESS = 0x20
APPEND_WRITE = 0x40

_DEFAULT_ATTRS = NON_VOLATILE | BOOTSERVICE_ACCESS

# 16 zero bytes — the "no namespace yet" seed for an enumeration walk.
_ZERO_GUID = b"\x00" * 16


def get(name, namespace):
    """Return a variable's data as bytes (raises uefi.Error if absent)."""
    return get_with_attributes(name, namespace)[1]


def get_with_attributes(name, namespace):
    """Return (attributes, data_bytes) for a variable."""
    st, attrs, data = raw.get_variable(name, GUID(namespace).bytes)
    status.check(st)
    return attrs, data


def set(name, namespace, data, attributes=_DEFAULT_ATTRS):
    """Create or replace a variable."""
    status.check(raw.set_variable(name, GUID(namespace).bytes, attributes, data))


def delete(name, namespace):
    """Delete a variable (a no-op if it doesn't exist)."""
    st = raw.set_variable(name, GUID(namespace).bytes, 0, None)
    if st != status.NOT_FOUND:
        status.check(st)


def exists(name, namespace):
    """True if the variable is present."""
    st, _attrs, _data = raw.get_variable(name, GUID(namespace).bytes)
    if st == status.SUCCESS:
        return True
    if st == status.NOT_FOUND:
        return False
    status.check(st)  # surface unexpected errors


def enumerate(namespace=None):
    """List (name, GUID) for every variable, optionally filtered to a namespace.

    Collect the full list before creating/deleting variables — stepping the walk
    while mutating the variable store is undefined (see pitfalls).
    """
    want = GUID(namespace) if namespace is not None else None
    out = []
    name = ""
    g = _ZERO_GUID
    while True:
        st, next_name, next_guid = raw.get_next_variable_name(name, g)
        if st == status.NOT_FOUND:
            break
        status.check(st)
        gobj = GUID(next_guid)
        if want is None or gobj == want:
            out.append((next_name, gobj))
        name = next_name
        g = next_guid
    return out


# ── Variable Policy (EDK2 EFI_VARIABLE_POLICY_PROTOCOL) ──────────────────────
# LockPolicyType values (VARIABLE_POLICY_TYPE_*).
LOCK_NONE = 0
LOCK_NOW = 1
LOCK_ON_CREATE = 2
LOCK_ON_VAR_STATE = 3

_ENTRY_REVISION = const(0x00010000)  # VARIABLE_POLICY_ENTRY_REVISION
_ENTRY_HEADER = const(44)  # sizeof(VARIABLE_POLICY_ENTRY) — name follows

VARIABLE_POLICY = ProtocolDescriptor(
    "EDKII_VARIABLE_POLICY_PROTOCOL",
    _guid.VARIABLE_POLICY_PROTOCOL,
    [
        Field("revision", U64),
        Method("disable_variable_policy", ()),
        Method("is_variable_policy_enabled", (VPtr,)),  # (BOOLEAN *State)
        Method("register_variable_policy", (VPtr,)),  # (VARIABLE_POLICY_ENTRY *)
        Method("dump_variable_policy", (VPtr, VPtr)),
        Method("lock_variable_policy", ()),
    ],
)

_policy = None


def _proto():
    global _policy
    if _policy is None:
        _policy = VARIABLE_POLICY.locate()  # raises uefi.Error if absent
    return _policy


def _le16(v):
    return bytes((v & 0xFF, (v >> 8) & 0xFF))


def _le32(v):
    return bytes((v & 0xFF, (v >> 8) & 0xFF, (v >> 16) & 0xFF, (v >> 24) & 0xFF))


class VariablePolicy:
    """Wraps the EDK2 Variable Policy protocol (absent on some firmware)."""

    @staticmethod
    def available():
        try:
            _proto()
            return True
        except status.Error:
            return False

    @staticmethod
    def is_enabled():
        out = bytearray(1)
        _proto().is_variable_policy_enabled(out)
        return out[0] != 0

    @staticmethod
    def register(
        namespace,
        name,
        *,
        min_size=0,
        max_size=0xFFFFFFFF,
        attributes_must=0,
        attributes_cant=0,
        lock=LOCK_NONE,
    ):
        """Register a policy on a variable (or, with name=None, a namespace)."""
        if lock == LOCK_ON_VAR_STATE:
            raise NotImplementedError("LOCK_ON_VAR_STATE policies not yet supported")
        ns = GUID(namespace).bytes
        name_u16 = utf16.encode(name) if name is not None else b"\x00\x00"
        total = _ENTRY_HEADER + len(name_u16)
        # VARIABLE_POLICY_ENTRY: Version, Size, OffsetToName, Namespace(16),
        # MinSize, MaxSize, AttributesMustHave, AttributesCantHave,
        # LockPolicyType + 3 padding bytes, then the CHAR16 name.
        entry = (
            _le32(_ENTRY_REVISION)
            + _le16(total)
            + _le16(_ENTRY_HEADER)
            + ns
            + _le32(min_size)
            + _le32(max_size)
            + _le32(attributes_must)
            + _le32(attributes_cant)
            + bytes((lock & 0xFF, 0, 0, 0))
            + name_u16
        )
        _proto().register_variable_policy(entry)

    @staticmethod
    def lock_interface():
        """Lock the policy engine — no further policy edits until reset."""
        _proto().lock_variable_policy()
