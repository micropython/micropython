# Coercion class 7: out-parameters are declared in the descriptor, not hand-rolled at
# the call site. An Out(type) member makes the stub allocate the scratch buffer, pass
# its address, and return the decoded value(s) — so no caller writes bytearray(8)
# followed by int.from_bytes ever again.
try:
    import uefi
except ImportError:
    print("SKIP")
    raise SystemExit

# Not part of the SKIP guard: a missing Out is the failure under test, not a reason
# to skip the file.
from uefi.protocol import ProtocolDescriptor, Field, Method, Out, U64, VPtr

# ── two Out(U64)s come back as a tuple ──────────────────────────────────────
c = uefi.Console()
cols, rows = c.out.query_mode(c.current_mode())
print(cols > 0 and rows > 0)
print((cols, rows) == c.size())  # the wrapper reads the same way

# ── a single Out is returned unwrapped ──────────────────────────────────────
h = uefi.handle.locate_handles(uefi.guid.SIMPLE_FILE_SYSTEM_PROTOCOL)[0]
fs = uefi.protocols.SIMPLE_FS.open(h)
root = fs.open_volume()
print(isinstance(root, int) and root != 0)
fs.close()

# ── Out() is part of the public DSL, usable in a hand-written descriptor ────
mine = ProtocolDescriptor(
    "EFI_SIMPLE_FILE_SYSTEM_PROTOCOL",
    uefi.guid.SIMPLE_FILE_SYSTEM_PROTOCOL,
    [Field("revision", U64), Method("open_volume", (Out(VPtr),))],
)
w = mine.open(h)
print(w.open_volume() != 0)
w.close()

# ── check=False keeps the status, with the outs after it ───────────────────
# COMPONENT_NAME2.get_driver_name is (Str, Out(VPtr)) -> (status, CHAR16*)
name = uefi.driver.driver_name(uefi.handle.locate_handles(uefi.guid.DRIVER_BINDING_PROTOCOL)[0])
print(name is None or isinstance(name, str))

# ── four Out(U64)s: PCI_IO.get_location, where a PCI bus exists ─────────────
pci = uefi.handle.locate_handles(uefi.guid.PCI_IO_PROTOCOL)
if pci:
    p = uefi.protocols.PCI_IO.open(pci[0])
    loc = p.get_location()
    print(len(loc) == 4 and all(isinstance(v, int) for v in loc))
    p.close()
else:
    print(True)

# ── Out(Bool) decodes to a bool ────────────────────────────────────────────
if uefi.VariablePolicy.available():
    print(isinstance(uefi.VariablePolicy.is_enabled(), bool))
else:
    print(True)

print("uefi out params ok")
