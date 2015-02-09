# test construction of bytes from different objects

from array import array

# tuple, list, bytearray
print(bytes((1, 2)))
print(bytes([1, 2]))
print(bytes(bytearray(4)))

# arrays
print(bytes(array('b', [1, 2])))
print(bytes(array('h', [1, 2])))
print(bytes(array('I', [1, 2])))

# long ints
print(ord(bytes([14953042807679334000 & 0xff])))
