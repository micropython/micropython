# test construction of array.array from different objects

try:
    from array import array
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

# raw copy from bytes, bytearray
print(array('h', b'12'))
