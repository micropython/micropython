# Regressions for defects found during the API type-coercion survey.
try:
    import uefi
    import gc
except ImportError:
    print("SKIP")
    raise SystemExit

# ── 1. Handle.device_path() returns None, it does not raise ─────────────────
# It documents "None if it has no Device Path protocol" and tested only for
# EFI_NOT_FOUND — but OpenProtocol reports EFI_UNSUPPORTED for a protocol that
# isn't on the handle, so walking the handle database used to throw.
with_dp = 0
without_dp = 0
for h in uefi.handle.all_handles():
    d = h.device_path()
    if d is None:
        without_dp += 1
    else:
        with_dp += 1
print(with_dp > 0 and without_dp > 0)
print(uefi.handle.locate_handles(uefi.guid.DEVICE_PATH_PROTOCOL)[0].device_path() is not None)

# ── 2. connect(remaining_path=...) keeps the packed path alive ──────────────
# The address handed to ConnectController used to come from an unreferenced
# temporary. Collecting between building the path and the call is the shape of
# the failure; a firmware status is a fine outcome, a crash is not.
h = uefi.handle.locate_handles(uefi.guid.DEVICE_PATH_PROTOCOL)[0]
dp = h.device_path()
gc.collect()
try:
    uefi.driver.connect(h, remaining_path=dp)
    print(True)
except uefi.Error:
    print(True)

# ── 3. a pointer argument never exposes an immutable bytes to firmware ──────
# Read-only byte arguments are legitimate, so they are copied into writable
# scratch rather than handed over directly: firmware can never scribble on an
# interned bytes object.
BLOB = b"\x7f\xff\x04\x00"
try:
    tt = uefi.device_path.DEVICE_PATH_TO_TEXT.locate()
    tt.convert_device_path_to_text(BLOB, False, False)
except uefi.Error:
    pass
print(BLOB == b"\x7f\xff\x04\x00")

print("uefi survey bugs ok")
