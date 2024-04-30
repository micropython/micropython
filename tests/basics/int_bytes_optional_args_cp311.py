# Check optional byteorder argument (CPython 3.11+)
print((10).to_bytes(1))
print((100).to_bytes(10))
print(int.from_bytes(b"\0\0\0\0\0\0\0\0\0\x01"))
print(int.from_bytes(b"\x01\0"))

# Check optional length argument (CPython 3.11+)
print((10).to_bytes())
print((100).to_bytes())
