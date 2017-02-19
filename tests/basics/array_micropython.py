# test MicroPython-specific features of array.array
try:
    import array
except ImportError:
    import sys
    print("SKIP")
    sys.exit()

# arrays of objects
a = array.array('O')
a.append(1)
print(a[0])

# arrays of pointers
a = array.array('P')
a.append(1)
print(a[0])
