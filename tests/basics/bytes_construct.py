# test construction of bytes from different objects

from array import array

# tuple, list, bytearray
print(bytes((1, 2)))
print(bytes([1, 2]))
print(bytes(bytearray(4)))

# arrays
print(bytes(array('b', [1, 2])))
print(bytes(array('h', [0x101, 0x202])))

# constructor value out of range
try:
    bytes([-1])
except ValueError:
    print('ValueError')

# constructor value out of range
try:
    bytes([256])
except ValueError:
    print('ValueError')

# error in construction
try:
    a = bytes([1, 2, 3], 1)
except TypeError:
    print('TypeError')
