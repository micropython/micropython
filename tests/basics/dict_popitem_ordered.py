# Check if dicts preserve insertion order (MICROPY_PY_MAP_ORDERED).
if list({2: 0, 1: 0, 3: 0}.keys()) != [2, 1, 3]:
    print("SKIP")
    raise SystemExit


# Test dict.popitem() returns items in LIFO order and leaves the dict consistent.

# Pop entire dict, verify all entries are returned in LIFO order.
d = {}
for i in range(5):
    d[i] = i * 10
items = []
while d:
    items.append(d.popitem())
print("all items:", items)

# Interleaved popitem and add: verify remaining dict is consistent.
d = {1: 10, 2: 20, 3: 30}
print(d.popitem())
d[4] = 40
d[5] = 50
print(d.popitem())
print(d.popitem())
print("remaining:", sorted(d.items()))

# popitem after del (mixed operations).
d = {}
for i in range(8):
    d[i] = i
del d[3]
del d[5]
# Remaining: 0,1,2,4,6,7 -- popitem should pop from end.
p1 = d.popitem()
p2 = d.popitem()
print("after del pops:", p1, p2)
# Verify remaining keys are accessible.
for k in list(d.keys()):
    assert d[k] == k, "key {} has wrong value after mixed ops".format(k)
print("remaining keys:", sorted(d.keys()))

# Pop everything after deletes.
d = {1: 10, 2: 20, 3: 30, 4: 40, 5: 50}
del d[2]
del d[4]
items = []
while d:
    items.append(d.popitem())
print("pop-all after del:", items)

# Verify popitem on empty dict raises KeyError.
try:
    {}.popitem()
except KeyError:
    print("empty popitem: KeyError")
