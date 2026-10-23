# Test dict operations under heap lock.
# The ordered hash table implementation must handle delete+add cycles
# without requiring heap allocation (rehash) by compacting in-place.

try:
    import micropython
except ImportError:
    print("SKIP")
    raise SystemExit

# Check for stackless build, which can't call functions without
# allocating a frame on heap.
try:

    def stackless():
        pass

    micropython.heap_lock()
    stackless()
    micropython.heap_unlock()
except RuntimeError:
    print("SKIP")
    raise SystemExit

# Test 1: delete+add cycles under heap lock.
d = {}
for i in range(6):
    d[i] = i

micropython.heap_lock()
try:
    for i in range(6, 20):
        del d[i - 6]
        d[i] = i
    print("dict delete+add cycles: OK")
except MemoryError:
    print("dict delete+add cycles: MemoryError")
micropython.heap_unlock()

# Verify contents are correct after heap-locked cycles.
print("post-lock contents:", sorted(d.keys()), sorted(d.values()))

# Test 2: Same with smaller dict.
gd = {}
for i in range(4):
    gd[i] = i

micropython.heap_lock()
try:
    for i in range(4, 12):
        del gd[i - 4]
        gd[i] = i
    print("small dict cycles: OK")
except MemoryError:
    print("small dict cycles: MemoryError")
micropython.heap_unlock()

# Test 3: Delete all entries then add under heap lock (all-tombstone compact).
d3 = {}
for i in range(6):
    d3[i] = i
micropython.heap_lock()
try:
    for i in range(6):
        del d3[i]
    # Dict is now empty but dense array is full of tombstones.
    # Adding should trigger in-place compact (filled=0 < used=6), then succeed.
    d3["new"] = 99
    print("all-tombstone add: OK")
except MemoryError:
    print("all-tombstone add: MemoryError")
micropython.heap_unlock()
print("after all-tombstone:", list(d3.keys()), list(d3.values()))
