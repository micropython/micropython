# test construction of bytearray from different objects

from array import array

# arrays
print(bytearray(array('h', [1, 2])))
print(bytearray(array('I', [1, 2])))
