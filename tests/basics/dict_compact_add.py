# Check if dicts preserve insertion order (MICROPY_PY_MAP_ORDERED).
if list({2: 0, 1: 0, 3: 0}.keys()) != [2, 1, 3]:
    print("SKIP")
    raise SystemExit


# Test that in-place compact triggered by add (dense array full with tombstones)
# correctly preserves all entries and allows the new add to succeed.

# Fill a dict, delete entries to create tombstones, then add enough new entries
# to trigger compact-on-add (wrap-around in hash probe). Verify all entries.
d = {}
for i in range(10):
    d[i] = i * 10

# Delete half the entries (creates tombstones in dense array).
for i in range(0, 10, 2):
    del d[i]

# Add new entries. Eventually the dense array fills with tombstones + live entries,
# triggering in-place compact on wrap-around instead of rehash.
for i in range(100, 120):
    d[i] = i * 10

# Verify all expected keys are present with correct values.
for i in range(1, 10, 2):
    assert d[i] == i * 10, "original key {} has wrong value".format(i)
for i in range(100, 120):
    assert d[i] == i * 10, "new key {} has wrong value".format(i)

# Verify deleted keys are gone.
for i in range(0, 10, 2):
    assert i not in d, "deleted key {} still present".format(i)

print("compact-on-add: OK")
print("len:", len(d))

# Test insertion order is preserved after compact-on-add.
keys = list(d.keys())
expected = [1, 3, 5, 7, 9] + list(range(100, 120))
assert keys == expected, "order wrong: {}".format(keys)
print("order: OK")


# Test with non-qstr keys (tuples have __hash__ via mp_unary_op).
d2 = {}
for i in range(8):
    d2[(i, "x")] = i

for i in range(0, 8, 2):
    del d2[(i, "x")]

for i in range(100, 110):
    d2[(i, "x")] = i

# Verify all entries.
for i in range(1, 8, 2):
    assert d2[(i, "x")] == i
for i in range(100, 110):
    assert d2[(i, "x")] == i

print("non-qstr keys: OK")


# Test hash index rebuild correctness after compact.
# After compact, all probe chains in the hash index are rebuilt from scratch.
# Use enough entries that the hash index is stressed across multiple probe steps.
d3 = {}
for i in range(12):
    d3[i] = i

for i in [0, 1, 2, 3]:
    del d3[i]

# Adding 10 more entries forces the dense array to fill and triggers compact.
# The rebuilt hash index must correctly resolve all existing keys.
for i in range(20, 30):
    d3[i] = i

for i in range(4, 12):
    assert d3[i] == i, "key {} not found after hash rebuild".format(i)
for i in range(20, 30):
    assert d3[i] == i, "new key {} not found after hash rebuild".format(i)
for i in range(0, 4):
    assert i not in d3

print("hash index rebuild: OK")
