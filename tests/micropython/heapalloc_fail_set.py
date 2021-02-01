# test handling of failed heap allocation with set

import micropython

# create set
x = 1
micropython.heap_lock()
try:
    {x}
except MemoryError:
    print("MemoryError: set create")
micropython.heap_unlock()

# set copy
s = {1, 2}
micropython.heap_lock()
try:
    s.copy()
except MemoryError:
    print("MemoryError: set copy")
micropython.heap_unlock()
