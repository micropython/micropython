# check that heap_lock/heap_unlock work as expected

import micropython

micropython.heap_lock()

try:
    print([])
except MemoryError:
    print('MemoryError')

micropython.heap_unlock()

print([])
