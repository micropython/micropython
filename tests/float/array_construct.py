# test construction of array from array with float type

try:
    from uarray import array
except ImportError:
    try:
        from array import array
    except ImportError:
        print("SKIP")
        raise SystemExit

print(array('f', array('h', [1, 2])))
print(array('d', array('f', [1, 2])))
