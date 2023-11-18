# test construction of array.array from different objects

try:
    from array import array
except ImportError:
    print("SKIP")
    raise SystemExit

# raw copy from bytes, bytearray
print(array('h', b'12'))
