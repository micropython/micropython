# test micropython.heap_locked()

import micropython

if not hasattr(micropython, "heap_locked"):
    print("SKIP")
    raise SystemExit

micropython.heap_lock()
print(micropython.heap_locked())
micropython.heap_unlock()
print(micropython.heap_locked())
