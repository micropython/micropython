import machine

# ---------------------------------------------------------------------------
# machine.unique_id() contract test.
# Verifies that unique_id is bytes, has expected 64-bit length (8 bytes),
# is non-zero, and remains stable across repeated calls on the same boot.
# ---------------------------------------------------------------------------

print("***** Test 1: machine.unique_id() contract *****")
uid0 = machine.unique_id()

ok = True
if not isinstance(uid0, bytes):
    ok = False

if len(uid0) != 8:
    ok = False

if not any(b != 0 for b in uid0):
    ok = False

for _ in range(5):
    if machine.unique_id() != uid0:
        ok = False
        break

print("PASS" if ok else "FAIL")
