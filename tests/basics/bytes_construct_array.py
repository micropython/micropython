# test construction of bytes from different objects
try:
    from uarray import array
except ImportError:
    try:
        from array import array
    except ImportError:
        print("SKIP")
        raise SystemExit

# arrays
print(bytes(array('b', [1, 2])))
print(bytes(array('h', [0x101, 0x202])))
