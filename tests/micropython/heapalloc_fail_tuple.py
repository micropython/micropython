# test handling of failed heap allocation with tuple

try:
    import micropython

    micropython.heap_lock
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

# create tuple
x = 1
micropython.heap_lock()
try:
    (x,)
except MemoryError:
    print("MemoryError: tuple create")
micropython.heap_unlock()
