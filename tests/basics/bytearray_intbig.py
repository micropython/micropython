# Skip if long ints are not supported.
try:
    # We have to use variables because 1 << 40 causes an exception on parse and
    # cannot be caught.
    x = 40
    x = 1 << x
except OverflowError:
    print("SKIP")
    import sys
    sys.exit(1)

print(bytearray(2**65 - (2**65 - 1)))
