# test construction of bytes from different objects

from array import array

# arrays
print(bytes(array('h', [1, 2])))
print(bytes(array('I', [1, 2])))
