# test construction of bytearray from array with float type

try:
    from uarray import array
except ImportError:
    try:
        from array import array
    except ImportError:
        print("SKIP")
        raise SystemExit

print(bytes(array("f", [1, 2.3])))
