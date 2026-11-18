# Test that the heap area selected with gc.area() is per thread.

import gc
import _thread

try:
    gc.area
except AttributeError:
    print("SKIP")
    raise SystemExit

if len(gc.areas()) < 3:
    print("SKIP")
    raise SystemExit

lock = _thread.allocate_lock()
results = []


def thread_entry():
    # A new thread starts with the default selection.
    b1 = bytearray(64)
    with gc.area(2):
        b2 = bytearray(64)
    with lock:
        results.append((gc.area_of(b1) != 1, gc.area_of(b2) == 2))


with gc.area(1):
    lock.acquire()
    _thread.start_new_thread(thread_entry, ())
    lock.release()
    while not results:
        b = bytearray(64)
    # This thread's selection is unchanged by the other thread's.
    print(gc.area_of(bytearray(64)) == 1)
print(results)
