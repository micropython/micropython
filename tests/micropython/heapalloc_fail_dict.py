# test handling of failed heap allocation with dict

import micropython

# create dict
x = 1
micropython.heap_lock()
try:
    {x:x}
except MemoryError:
    print('MemoryError: create dict')
micropython.heap_unlock()

# create dict view
x = {1:1}
micropython.heap_lock()
try:
    x.items()
except MemoryError:
    print('MemoryError: dict.items')
micropython.heap_unlock()
