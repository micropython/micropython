# test micropython.heap_locked()

try:
    import micropython

    micropython.heap_locked
except (AttributeError, ImportError):
    print("SKIP")
    raise SystemExit

micropython.heap_lock()
print(micropython.heap_locked())
micropython.heap_unlock()
print(micropython.heap_locked())
