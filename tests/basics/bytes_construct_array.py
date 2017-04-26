# test construction of bytes from different objects
try:
    from array import array
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

# arrays
print(bytes(array('b', [1, 2])))
print(bytes(array('h', [0x101, 0x202])))
