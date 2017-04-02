# test construction of bytearray from array with float type

try:
    from array import array
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

print(bytearray(array('f', [1, 2.3])))
