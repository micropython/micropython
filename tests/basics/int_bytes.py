print((10).to_bytes(1, "little"))
print((111111).to_bytes(4, "little"))
print((100).to_bytes(10, "little"))
print(int.from_bytes(b"\x00\x01\0\0\0\0\0\0", "little"))
print(int.from_bytes(b"\x01\0\0\0\0\0\0\0", "little"))
print(int.from_bytes(b"\x00\x01\0\0\0\0\0\0", "little"))

# check that extra zero bytes don't change the internal int value
print(int.from_bytes(bytes(20), "little") == 0)
print(int.from_bytes(b"\x01" + bytes(20), "little") == 1)
