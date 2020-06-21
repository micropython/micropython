# test int interacting with bytearray

# should accept bytearray as an argument
print(int(bytearray(b"123")))
print(int.from_bytes(bytearray(b"\x00\x01"), "little"))
