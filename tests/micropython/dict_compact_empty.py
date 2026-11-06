# Test that deleting the last element doesn't cause issues.
# Compaction is skipped when dict becomes empty (filled == 0).

# Test 1: Delete all entries one by one
d = {1: "a", 2: "b", 3: "c"}
del d[1]
del d[2]
del d[3]
print(len(d))

# Dict should work normally after being emptied
d[10] = "x"
print(d[10])
print(len(d))

# Test 2: Single element dict
d2 = {42: "only"}
del d2[42]
print(len(d2))
d2[100] = "new"
print(d2[100])

# Test 3: Empty via popitem
d3 = {"a": 1, "b": 2}
d3.popitem()
d3.popitem()
print(len(d3))
d3["c"] = 3
print(d3["c"])

# Test 4: Repeated empty/fill cycles
d4 = {}
for cycle in range(5):
    for i in range(10):
        d4[i] = cycle
    for i in range(10):
        del d4[i]
print("cycles OK, len:", len(d4))
