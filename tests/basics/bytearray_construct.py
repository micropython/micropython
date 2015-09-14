# test construction of bytearray from different objects

from array import array

# bytes, tuple, list
print(bytearray(b'123'))
print(bytearray((1, 2)))
print(bytearray([1, 2]))

# arrays
print(bytearray(array('b', [1, 2])))
print(bytearray(array('h', [0x101, 0x202])))
