# Coercion class 2: a uefi.GUID *is* a packed EFI_GUID buffer, so it goes anywhere
# 16 raw bytes go — bytes()/memoryview()/uctypes, and every raw entry point that
# takes a GUID — and the canonical string form is accepted there too.
try:
    import uefi
    import uctypes
except ImportError:
    print("SKIP")
    raise SystemExit

g = uefi.guid.BLOCK_IO_PROTOCOL
sform = "964e5b21-6459-11d2-8e39-00a0c969723b"

# ── a GUID is a buffer ──────────────────────────────────────────────────────
print(len(bytes(g)) == 16, bytes(g) == g.bytes)
print(len(memoryview(g)) == 16, bytes(memoryview(g)) == g.bytes)
print(uctypes.bytes_at(uctypes.addressof(g), 16) == g.bytes)

# ── every raw GUID parameter takes the object itself ────────────────────────
st, handles = uefi.raw.locate_handle_buffer(2, g)
print(st == uefi.status.SUCCESS and len(handles) > 0)
print(uefi.raw.locate_protocol(g)[0] == uefi.status.SUCCESS)

h0 = handles[0]
agent = uefi.raw.image_handle()
st, iface = uefi.raw.open_protocol(h0, g, agent, 0, uefi.handle.GET_PROTOCOL)
print(st == uefi.status.SUCCESS and iface != 0)
print(uefi.raw.open_protocol_information(h0, g)[0] == uefi.status.SUCCESS)
st = uefi.raw.close_protocol(h0, g, agent, 0)  # untracked GET_PROTOCOL open
print(st in (uefi.status.SUCCESS, uefi.status.NOT_FOUND))

# ── ... and the canonical string form, identically ──────────────────────────
print(uefi.raw.locate_handle_buffer(2, sform)[1] == handles)
print(uefi.raw.locate_protocol(sform)[0] == uefi.status.SUCCESS)
print(uefi.raw.locate_protocol("{964E5B21-6459-11D2-8E39-00A0C969723B}")[0] == uefi.status.SUCCESS)

# ── variable namespaces reach Runtime Services without .bytes ───────────────
st, _attrs, _data = uefi.raw.get_variable("BootCurrent", uefi.guid.GLOBAL_VARIABLE)
print(st in (uefi.status.SUCCESS, uefi.status.NOT_FOUND))
st, _name, _ns = uefi.raw.get_next_variable_name("", uefi.GUID(b"\x00" * 16))
print(st in (uefi.status.SUCCESS, uefi.status.NOT_FOUND))

# ── a bad GUID is still rejected, at both layers ────────────────────────────
try:
    uefi.raw.locate_protocol("not-a-guid")
    print("bad guid accepted")
except ValueError:
    print("ValueError")
try:
    uefi.raw.locate_protocol(b"\x01\x02\x03")
    print("short guid accepted")
except ValueError:
    print("ValueError")

print("uefi guid buffer ok")
