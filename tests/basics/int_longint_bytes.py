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

print((2**64).to_bytes(9, "little"))
print(int.from_bytes(b"\x00\x01\0\0\0\0\0\0", "little"))
print(int.from_bytes(b"\x01\0\0\0\0\0\0\0", "little"))
print(int.from_bytes(b"\x00\x01\0\0\0\0\0\0", "little"))
