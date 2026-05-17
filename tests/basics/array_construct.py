# test construction of array.array from different objects

try:
    from array import array
except ImportError:
    print("SKIP")
    raise SystemExit

# tuple, list
print(array('b', (1, 2)))
print(array('h', [1, 2]))

# raw copy from bytes, bytearray
print(array('h', b'22'))  # should be byteorder-neutral
print(array('h', bytearray(2)))
print(array('i', bytearray(4)))

# convert from other arrays
print(array('H', array('b', [1, 2])))
print(array('b', array('I', [1, 2])))
