# check that heap_lock/heap_unlock work as expected

try:
    import micropython
except ImportError:
    print("SKIP")
    raise SystemExit

l = []
l2 = list(range(100))

micropython.heap_lock()
micropython.heap_lock()

# general allocation on the heap
try:
    print([])
except MemoryError:
    print("MemoryError")

# expansion of a heap block
try:
    l.extend(l2)
except MemoryError:
    print("MemoryError")

print(micropython.heap_unlock())

# Should still fail
try:
    print([])
except MemoryError:
    print("MemoryError")

micropython.heap_unlock()

# check that allocation works after an unlock
print([])
