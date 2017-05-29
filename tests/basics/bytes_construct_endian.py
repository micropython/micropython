# test construction of bytes from different objects

try:
    from array import array
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

# arrays
print(bytes(array('h', [1, 2])))
print(bytes(array('I', [1, 2])))
