# Check if dicts preserve insertion order (MICROPY_PY_MAP_ORDERED).
if list({2: 0, 1: 0, 3: 0}.keys()) != [2, 1, 3]:
    print("SKIP")
    raise SystemExit


# Test that dict ordering is preserved after compaction.
# When tombstones exceed 50% of live entries, the dict compacts.
# This must preserve insertion order.

d = {}
for i in range(100):
    d[i] = i

# Delete 67 entries: filled=33, tombstones=67
# Threshold = 33/2 = 16, and 67 > 16, so compaction triggers
for i in range(67):
    del d[i]

# Remaining keys must be in insertion order
print(list(d.keys()))

# Values should also be correct (spot check)
print(d[67], d[80], d[99])

# Dict should still be usable after compaction
d[200] = 200
d[201] = 201
keys = list(d.keys())
print(keys[-3], keys[-2], keys[-1])

# Test with mixed key types (strings and ints)
d2 = {}
for i in range(20):
    d2[i] = i
    d2["key" + str(i)] = "val" + str(i)

# Delete enough to trigger compaction
for i in range(15):
    del d2[i]
    del d2["key" + str(i)]

# Remaining should preserve insertion order
print(list(d2.keys()))
