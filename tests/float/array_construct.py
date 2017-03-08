# test construction of array from array with float type

try:
    from array import array
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

print(array('f', array('h', [1, 2])))
print(array('d', array('f', [1, 2])))
