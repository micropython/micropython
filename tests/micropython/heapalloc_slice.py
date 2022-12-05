# slice operations that don't require allocation
try:
    from micropython import heap_lock, heap_unlock
except (ImportError, AttributeError):
    heap_lock = heap_unlock = lambda: 0

b = bytearray(range(10))

m = memoryview(b)

heap_lock()

b[3:5] = b"aa"
m[5:7] = b"bb"

heap_unlock()

print(b)
