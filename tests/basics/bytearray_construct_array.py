# test construction of bytearray from different objects
# requires: bytearray

try:
    from array import array
except ImportError:
    print("SKIP")
    raise SystemExit

# arrays
print(bytearray(array('b', [1, 2])))
print(bytearray(array('h', [0x101, 0x202])))
