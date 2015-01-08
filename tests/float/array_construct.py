# test construction of array from array with float type

from array import array

print(array('f', array('h', [1, 2])))
print(array('d', array('f', [1, 2])))
