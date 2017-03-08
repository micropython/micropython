# test construction of bytearray from different objects
try:
    from array import array
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

# arrays
print(bytearray(array('b', [1, 2])))
print(bytearray(array('h', [0x101, 0x202])))
