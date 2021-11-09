# test handling of failed heap allocation with tuple

from src import micropython

# create tuple
x = 1
micropython.heap_lock()
try:
    (x,)
except MemoryError:
    print("MemoryError: tuple create")
micropython.heap_unlock()
