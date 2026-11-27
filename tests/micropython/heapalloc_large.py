# Test that large single allocations succeed when the heap has room for them,
# including when the heap has to grow to fit them (split heap auto).

SIZES = (64 * 1024, 96 * 1024)

try:
    import gc

    gc.collect()
    if gc.mem_free() < 4 * max(SIZES):
        raise MemoryError
except (ImportError, AttributeError, MemoryError):
    print("SKIP")
    raise SystemExit

for size in SIZES:
    buf = bytearray(size)
    buf[-1] = 1
    print(size, len(buf), buf[-1])
    buf = None
    gc.collect()
