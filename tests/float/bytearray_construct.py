# test construction of bytearray from array with float type

from array import array

print(bytearray(array('f', [1, 2.3])))
