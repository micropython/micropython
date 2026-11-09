# Garbage collector benchmark.
#
# Times garbage collections, allocations and gc.mem_free() with different heap
# shapes, to compare GC implementations and configurations on one board.
#
# Run it on a board with:
#     mpremote run tests/gc_bench/gcbench.py
# or on the unix port with:
#     micropython -X heapsize=64M tests/gc_bench/gcbench.py [scale]
#
# The sizes of the data sets scale with the size of the heap, so that they use
# a similar fraction of it on every board.  An optional scale argument sets the
# scale directly instead; use the same scale when comparing results.

import gc, sys, time

try:
    ticks_us, ticks_diff = time.ticks_us, time.ticks_diff
except AttributeError:
    ticks_us = lambda: int(time.perf_counter() * 1e6)
    ticks_diff = lambda a, b: a - b

REPS = 5

gc.collect()
heap_size = gc.mem_free() + gc.mem_alloc()
if len(sys.argv) > 1:
    SCALE = float(sys.argv[1])
else:
    # A scale of 1 suits a heap of about 9 MB.  Larger scales add run time
    # without making the results more useful.
    SCALE = min(heap_size / 9_000_000, 8)

results = []


def scaled(n):
    return max(1, int(n * SCALE))


# Large buffers: 16 x 64 KB at a scale of 1, fewer or smaller otherwise.
if SCALE >= 1:
    n_bufs, buf_size = scaled(16), 64 * 1024
else:
    n_bufs, buf_size = 16, max(256, int(64 * 1024 * SCALE) // 256 * 256)


def median(xs):
    xs = sorted(xs)
    return xs[len(xs) // 2]


def time_collect():
    ts = []
    for _ in range(REPS):
        t = ticks_us()
        gc.collect()
        ts.append(ticks_diff(ticks_us(), t))
    return median(ts)


def report(name, us, extra=""):
    results.append((name, us))
    print("%-22s %11d us  %s" % (name, us, extra))


def fresh():
    gc.collect()
    gc.collect()


print("heap %d bytes, scale %.3f" % (heap_size, SCALE))

# A nearly empty heap: the fixed cost of a collection.
fresh()
report("collect_empty", time_collect())

# Many small live objects: marking a large object graph.
n = scaled(20000)
live = [[i, str(i), (i, i + 1)] for i in range(n)]
fresh()
report("collect_small_live", time_collect(), "%d lists" % n)
del live

# Large live buffers: free if the GC knows they hold no pointers.
fresh()
bufs = [bytearray(buf_size) for _ in range(n_bufs)]
report("collect_buffers_live", time_collect(), "%d bytes" % (n_bufs * buf_size))
del bufs

# Large dead buffers: sweeping long allocations.
fresh()
ts = []
for _ in range(REPS):
    junk = [bytearray(buf_size) for _ in range(n_bufs)]
    junk = None
    t = ticks_us()
    gc.collect()
    ts.append(ticks_diff(ticks_us(), t))
report("collect_buffers_dead", median(ts))

# Many small dead objects: sweeping a fragmented heap.
fresh()
ts = []
for _ in range(REPS):
    junk = [(i, i) for i in range(n)]
    junk = None
    t = ticks_us()
    gc.collect()
    ts.append(ticks_diff(ticks_us(), t))
report("collect_small_dead", median(ts), "%d tuples" % n)

# A long linked list: a deep object graph.
fresh()
head = None
for i in range(scaled(20000)):
    head = [i, head]
report("collect_deep_chain", time_collect(), "%d links" % scaled(20000))
del head

# Many lists that each hold 8 lists: a wide object graph.
fresh()
tree = [[[j] for j in range(8)] for i in range(scaled(3000))]
report("collect_wide_tree", time_collect(), "%d x 8 lists" % len(tree))
del tree

# A module-like dict of functions (200, or fewer on a small heap).  Each
# function references several objects, so this can overflow a small mark stack.
fresh()
n_funcs = max(20, min(200, scaled(2000)))
ns = {}
src = "\n".join("def f%d(x):\n    return x + %d" % (i, i) for i in range(n_funcs))
exec(src, ns)
del src
report("collect_funcs", time_collect(), "%d functions" % n_funcs)
del ns

# Multi-block allocations in a fragmented heap: small live objects with small
# holes between them, followed by allocations too big for any hole.
fresh()
keep = []
holes = []
for i in range(scaled(20000)):
    keep.append((i,))
    holes.append(bytearray(40))
holes = None
gc.collect()
got = []
t = ticks_us()
for i in range(scaled(2000)):
    got.append(bytearray(400))
report("alloc_fragmented_400B", ticks_diff(ticks_us(), t), "%d allocs" % len(got))
got = None
got = []
t = ticks_us()
try:
    for i in range(scaled(200)):
        got.append(bytearray(8192))
except MemoryError:
    pass
report("alloc_fragmented_8KB", ticks_diff(ticks_us(), t), "%d allocs" % len(got))
del keep, got

# Allocation throughput, including automatic collections.  This also depends
# on the speed of the VM, so it is less useful for comparing GCs on boards
# where code layout changes the speed of the VM.
fresh()
t = ticks_us()
for i in range(scaled(200000)):
    x = [i, i + 1]
report("churn_small", ticks_diff(ticks_us(), t), "%d allocs" % scaled(200000))

# gc.mem_free() with many allocations.
fresh()
filler = [bytearray(100) for _ in range(scaled(5000))]
t = ticks_us()
for _ in range(REPS):
    gc.mem_free()
report("mem_free_x%d" % REPS, ticks_diff(ticks_us(), t))
del filler

print("total %d us" % sum(r[1] for r in results))
