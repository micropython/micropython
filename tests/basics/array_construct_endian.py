# test construction of array.array from different objects

from array import array

# raw copy from bytes, bytearray
print(array('h', b'12'))
