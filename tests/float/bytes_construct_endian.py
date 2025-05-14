# test construction of bytes from array with float type

try:
    from array import array
except ImportError:
    print("SKIP")
    raise SystemExit

print(bytes(array("f", [1, 2.5])))
