# test handling of failed heap allocation with bytearray

import micropython

class GetSlice:
    def __getitem__(self, idx):
        return idx
sl = GetSlice()[:]

# create bytearray
micropython.heap_lock()
try:
    bytearray(4)
except MemoryError:
    print('MemoryError: bytearray create')
micropython.heap_unlock()

# create bytearray from bytes
micropython.heap_lock()
try:
    bytearray(b'0123')
except MemoryError:
    print('MemoryError: bytearray create from bytes')
micropython.heap_unlock()

# create bytearray from iterator
r = range(4)
micropython.heap_lock()
try:
    bytearray(r)
except MemoryError:
    print('MemoryError: bytearray create from iter')
micropython.heap_unlock()

# bytearray add
b = bytearray(4)
micropython.heap_lock()
try:
    b + b'01'
except MemoryError:
    print('MemoryError: bytearray.__add__')
micropython.heap_unlock()

# bytearray iadd
b = bytearray(4)
micropython.heap_lock()
try:
    b += b'01234567'
except MemoryError:
    print('MemoryError: bytearray.__iadd__')
micropython.heap_unlock()
print(b)

# bytearray append
b = bytearray(4)
micropython.heap_lock()
try:
    for i in range(100):
        b.append(1)
except MemoryError:
    print('MemoryError: bytearray.append')
micropython.heap_unlock()

# bytearray extend
b = bytearray(4)
micropython.heap_lock()
try:
    b.extend(b'01234567')
except MemoryError:
    print('MemoryError: bytearray.extend')
micropython.heap_unlock()

# bytearray get with slice
b = bytearray(4)
micropython.heap_lock()
try:
    b[sl]
except MemoryError:
    print('MemoryError: bytearray subscr get')
micropython.heap_unlock()

# extend bytearray using slice subscr
b = bytearray(4)
micropython.heap_lock()
try:
    b[sl] = b'01234567'
except MemoryError:
    print('MemoryError: bytearray subscr grow')
micropython.heap_unlock()
print(b)
