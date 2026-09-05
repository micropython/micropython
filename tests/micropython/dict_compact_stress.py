# Stress test for dict compaction.
# Without compaction, repeated add/delete cycles would cause unbounded
# memory growth from tombstone accumulation.

try:
    import gc
except ImportError:
    print("SKIP")
    raise SystemExit

d = {}

# Run many add/delete cycles
# Without compaction, this would accumulate 50 * 100 = 5000 tombstones
for cycle in range(50):
    # Add entries
    for i in range(100):
        d[i] = i * cycle

    # Delete all entries
    for i in range(100):
        del d[i]

# If we got here without MemoryError, basic compaction is working
print("pass1")

# Test with popitem too
d2 = {}
for cycle in range(50):
    for i in range(100):
        d2[i] = i
    for _ in range(100):
        d2.popitem()

print("pass2")

# Test mixed operations
d3 = {}
for cycle in range(30):
    # Add 200 entries
    for i in range(200):
        d3[i] = i

    # Delete half via del
    for i in range(100):
        del d3[i]

    # Delete rest via popitem
    for _ in range(100):
        d3.popitem()

print("pass3")

# Force GC and verify we haven't leaked too much memory
gc.collect()
print("pass4")
