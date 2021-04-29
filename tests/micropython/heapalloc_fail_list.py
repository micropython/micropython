# test handling of failed heap allocation with list

import micropython


class GetSlice:
    def __getitem__(self, idx):
        return idx


sl = GetSlice()[:]

# create slice in VM
l = [1, 2, 3]
micropython.heap_lock()
try:
    print(l[0:1])
except MemoryError:
    print("MemoryError: list index")
micropython.heap_unlock()

# get from list using slice
micropython.heap_lock()
try:
    l[sl]
except MemoryError:
    print("MemoryError: list get slice")
micropython.heap_unlock()

# extend list using slice subscr
l = [1, 2]
l2 = [3, 4, 5, 6, 7, 8, 9, 10]
micropython.heap_lock()
try:
    l[sl] = l2
except MemoryError:
    print("MemoryError: list extend slice")
micropython.heap_unlock()
print(l)
