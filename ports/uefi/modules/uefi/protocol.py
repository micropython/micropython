# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# uefi.protocol — descriptor-driven protocol overlays + trampoline dispatch.
#
# A ProtocolDescriptor declares a UEFI interface struct as an ordered list of
# Field(name, type) and Method(name, arg_types) members. From that it builds:
#   * a uctypes layout dict (offsets computed for the target's natural alignment)
#     so data fields can be read/written via a memory overlay, and
#   * method stubs that read the function pointer at the right slot and dispatch
#     through uefi.raw.call (the EFIAPI trampoline) — the one thing uctypes cannot
#     do itself.
#
# Method arguments are declared by type atom and coerced on the way through, so a
# pointer slot takes any .ptr-bearing object or byte block, a Guid slot takes any
# GUID spelling, and a WStr slot takes a str. An argument the method *writes to* is
# declared Out(type): the stub owns the scratch buffer and returns the decoded
# value, so out-parameters read as return values (see Out).
#
# Pointer/integer arguments only (UEFI methods never take floats/structs by value),
# matching the trampoline's contract. See README.md.

import uctypes

from . import raw
from . import status
from . import handle as _handle
from . import utf16
from . import coerce
from .guid import GUID

_LE = uctypes.LITTLE_ENDIAN


# ── Type atoms ───────────────────────────────────────────────────────────────
# Each atom carries its size/alignment, the uctypes scalar code used to read it as
# a field, and a `kind` that drives argument coercion for method calls.
class _Type:
    def __init__(self, size, code, kind):
        self.size = size
        self.align = size
        self.code = code
        self.kind = kind  # "int" | "ptr" | "guid" | "wstr" | "str"


U8 = _Type(1, uctypes.UINT8, "int")
I8 = _Type(1, uctypes.INT8, "int")
U16 = _Type(2, uctypes.UINT16, "int")
I16 = _Type(2, uctypes.INT16, "int")
U32 = _Type(4, uctypes.UINT32, "int")
I32 = _Type(4, uctypes.INT32, "int")
U64 = _Type(8, uctypes.UINT64, "int")
I64 = _Type(8, uctypes.INT64, "int")
Bool = _Type(1, uctypes.UINT8, "int")

# Pointers are all word-sized (both target arches are 64-bit). As fields they read
# back as the raw address (UINT64); as method args they coerce per `kind`.
VPtr = _Type(8, uctypes.UINT64, "ptr")  # void*
WStr = _Type(8, uctypes.UINT64, "wstr")  # CHAR16* (UTF-16LE)
Str = _Type(8, uctypes.UINT64, "str")  # CHAR8*  (UTF-8/ASCII)
Status = _Type(8, uctypes.UINT64, "int")  # UINTN return, checked by default
Guid = _Type(8, uctypes.UINT64, "guid")  # EFI_GUID*, coerced from any spelling


def Ptr(target=None):
    # Typed pointer. `target` (an optional descriptor) is advisory for now; the
    # field still reads back as a raw address. Kept for descriptor readability.
    return VPtr


class Out:
    """An output argument: the callee writes through the pointer we pass.

    An `Out(U64)` slot is filled by the method, not supplied by the caller: the
    stub allocates the scratch buffer, passes its address, and returns the value
    decoded as the declared type. A method with one Out returns that value; with
    several, a tuple of them. When the method is `check=False` the raw return
    value comes first, so the status is not lost.
    """

    def __init__(self, type=VPtr):
        self.type = type
        self.size = type.size
        self.align = type.align
        self.code = type.code
        self.kind = "out"


# ── Members ──────────────────────────────────────────────────────────────────
class Field:
    def __init__(self, name, type):
        self.name = name
        self.type = type


class Method:
    # `this=False` for stateless "global" protocols whose methods take NO leading
    # interface pointer (e.g. DevicePathToText/FromText, Timestamp) — the stub then
    # omits self_ptr from the call.
    def __init__(self, name, arg_types=(), ret=Status, check=True, this=True):
        self.name = name
        self.arg_types = tuple(arg_types)
        self.ret = ret
        self.check = check
        self.this = this


def _align(off, a):
    return (off + a - 1) & ~(a - 1)


def _build_layout(members):
    # Walk the members in declaration order, accumulating offsets with natural
    # alignment (matching how a C compiler lays out the interface struct). Methods
    # occupy a word-sized function-pointer slot, stored under a "__fn_<name>" key.
    # (Out() only ever appears in a Method's arg_types, never as a Field.)
    layout = {}
    methods = {}
    off = 0
    for m in members:
        if isinstance(m, Method):
            off = _align(off, 8)
            methods[m.name] = (off, m)
            layout["__fn_" + m.name] = uctypes.UINT64 | off
            off += 8
        else:
            t = m.type
            off = _align(off, t.align)
            layout[m.name] = t.code | off
            off += t.size
    return layout, methods, off


# ── Argument coercion ────────────────────────────────────────────────────────
def _coerce(t, v, keep):
    kind = t.kind
    if kind == "int":
        return int(v)
    if kind == "ptr":
        if v is None:
            return 0
        p = getattr(v, "ptr", None)
        if p is not None:
            return int(p)
        if isinstance(v, int):
            return v
        # Any byte block: bytes/bytearray/memoryview, a GUID, a DevicePath, ...
        # Immutable bytes are copied first — firmware writing through the address
        # of an interned bytes object would corrupt every other reference to it.
        buf = coerce.as_writable(v)
        keep.append(buf)
        return uctypes.addressof(buf)
    if kind == "guid":
        if v is None:
            return 0
        g = GUID(v)  # a GUID, 16 bytes, or a string
        keep.append(g)
        return uctypes.addressof(g)
    if kind == "wstr":
        if v is None:
            return 0
        buf = utf16.encode(v)
        keep.append(buf)
        return uctypes.addressof(buf)
    if kind == "str":
        if v is None:
            return 0
        buf = v.encode() + b"\x00"
        keep.append(buf)
        return uctypes.addressof(buf)
    return int(v)


def _decode_out(t, buf):
    # Read an Out slot's scratch buffer back as the declared type.
    v = int.from_bytes(buf, "little")
    if t is Bool:
        return v != 0
    if t is I8 and v >= 0x80:
        return v - 0x100
    if t is I16 and v >= 0x8000:
        return v - 0x10000
    if t is I32 and v >= 0x80000000:
        return v - 0x100000000
    if t is I64 and v >= 0x8000000000000000:
        return v - 0x10000000000000000
    return v


def _coerce_args(arg_types, args, keep, outs):
    # Out slots are not supplied by the caller, so only the in slots are counted.
    n_in = 0
    for t in arg_types:
        if t.kind != "out":
            n_in += 1
    if len(args) != n_in:
        raise TypeError("expected %d args, got %d" % (n_in, len(args)))
    cargs = []
    i = 0
    for t in arg_types:
        if t.kind == "out":
            buf = bytearray(t.size)
            keep.append(buf)
            outs.append((t.type, buf))
            cargs.append(uctypes.addressof(buf))
        else:
            cargs.append(_coerce(t, args[i], keep))
            i += 1
    return cargs


# ── Wrapper ──────────────────────────────────────────────────────────────────
class ProtocolWrapper:
    # Overlays a descriptor on an interface pointer. Data fields are read/written
    # as attributes (via uctypes); declared methods dispatch through raw.call.
    def __init__(self, descriptor, iface_ptr, protocol_handle=None):
        self._desc = descriptor
        self.ptr = int(iface_ptr)
        self._ph = protocol_handle
        self._s = uctypes.struct(self.ptr, descriptor._layout, _LE)

    def __getattr__(self, name):
        desc = self._desc
        methods = desc._methods
        if name in methods:
            meth = methods[name][1]
            fn = getattr(self._s, "__fn_" + name)
            self_ptr = self.ptr

            def _call(*args, _fn=fn, _meth=meth, _self_ptr=self_ptr):
                keep = []
                outs = []
                cargs = _coerce_args(_meth.arg_types, args, keep, outs)
                if _meth.this:
                    r = raw.call(_fn, _self_ptr, *cargs)
                else:
                    r = raw.call(_fn, *cargs)
                if _meth.check:
                    status.check(r)
                if not outs:
                    return r
                # Out slots (see Out): the decoded values replace the return, with
                # the raw return kept in front when it wasn't checked for us.
                vals = [_decode_out(t, buf) for t, buf in outs]
                if not _meth.check:
                    return tuple([r] + vals)
                return vals[0] if len(vals) == 1 else tuple(vals)

            return _call
        # Otherwise a data field — let uctypes resolve it (AttributeError if not).
        return getattr(self._s, name)

    def __setattr__(self, name, value):
        # Instance bookkeeping attrs are set normally; everything else is a field.
        if name in ("_desc", "ptr", "_ph", "_s"):
            object.__setattr__(self, name, value)
        else:
            setattr(self._s, name, value)

    def __int__(self):
        # The `.ptr` contract in integer form: the interface pointer (see
        # uefi.handle.Handle), so a wrapper can be handed to raw calls directly.
        return self.ptr

    def struct_at(self, field, descriptor):
        # Overlay another descriptor on a pointer-typed field of this interface.
        addr = getattr(self._s, field)
        if not addr:
            raise ValueError("null pointer: " + field)
        return descriptor.wrap(addr)

    def close(self):
        if self._ph is not None:
            self._ph.close()
            self._ph = None

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        self.close()

    def __del__(self):
        self.close()

    def __repr__(self):
        return "<%s @ 0x%x>" % (self._desc.name, self.ptr)


# ── Descriptor ───────────────────────────────────────────────────────────────
class ProtocolDescriptor:
    def __init__(self, name, guid, members):
        self.name = name
        # Coerce here too, so a descriptor built from a string/bytes GUID works and
        # `.guid` is always a GUID (None for nested/anonymous structs).
        self.guid = GUID(guid) if guid is not None else None
        self.members = members
        self._layout, self._methods, self.size = _build_layout(members)

    def wrap(self, iface_ptr, protocol_handle=None):
        # Overlay on an interface pointer we already hold (no open/close ownership
        # unless a ProtocolHandle is supplied). `iface_ptr` may be the raw address
        # or any object carrying it as `.ptr` — a ProtocolHandle, say.
        return ProtocolWrapper(self, iface_ptr, protocol_handle)

    def open(self, handle, attributes=_handle.GET_PROTOCOL):
        # `handle` is anything Handle() accepts: a Handle, a bare EFI_HANDLE int
        # (as read out of a VPtr field), or any .ptr-bearing object.
        ph = _handle.ProtocolHandle.open(handle, self.guid, attributes)
        return ProtocolWrapper(self, ph.ptr, ph)

    def locate(self):
        ph = _handle.ProtocolHandle.locate(self.guid)
        return ProtocolWrapper(self, ph.ptr, ph)

    def locate_all(self):
        out = []
        for h in _handle.locate_handles(self.guid):
            ph = _handle.ProtocolHandle.open(h, self.guid)
            out.append(ProtocolWrapper(self, ph.ptr, ph))
        return out

    def __repr__(self):
        return "ProtocolDescriptor(%r)" % self.name
