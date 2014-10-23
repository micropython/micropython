# test memoryview

# test reading from bytes
b = b'1234'
m = memoryview(b)
print(len(m))
print(m[0], m[1], m[-1])
print(list(m))

# test writing to bytes
try:
    m[0] = 1
except TypeError:
    print("TypeError")

# test writing to bytearray
b = bytearray(b)
m = memoryview(b)
m[0] = 1
print(b)
print(list(m))

# test slice
m = memoryview(b'1234')
print(list(m[1:]))
print(list(m[1:-1]))

import array
a = array.array('i', [1, 2, 3, 4])
m = memoryview(a)
print(list(m))
print(list(m[1:-1]))
m[2] = 6
print(a)
