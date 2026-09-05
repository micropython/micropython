# Check if dicts preserve insertion order (MICROPY_PY_MAP_ORDERED).
if list({2: 0, 1: 0, 3: 0}.keys()) != [2, 1, 3]:
    print("SKIP")
    raise SystemExit

# Test compaction threshold boundary.
# Compaction triggers when: tombstones > filled / 2

# Start with 30 entries
d = {}
for i in range(30):
    d[i] = i

# Delete entries one by one and verify dict remains consistent
for i in range(25):
    del d[i]
    # Verify remaining entries are correct
    expected = list(range(i + 1, 30))
    if list(d.keys()) != expected:
        print("FAIL at deletion", i)
        break
else:
    print("sequential delete OK")

# Verify values are still correct after deletions (spot check)
d2 = {}
for i in range(50):
    d2[i] = i * 2
for i in range(40):
    del d2[i]

print(list(d2.keys()))
print(d2[40], d2[45], d2[49])

# Test with different deletion patterns (non-sequential)
d3 = {}
for i in range(100):
    d3[i] = i
# Delete every other entry
for i in range(0, 100, 2):
    del d3[i]

# Should have odd numbers in order
keys = list(d3.keys())
print(keys[0], keys[1], keys[2], keys[3], keys[4], keys[5], keys[6], keys[7], keys[8], keys[9])
print(keys[-5], keys[-4], keys[-3], keys[-2], keys[-1])
