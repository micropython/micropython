# test bytes + bytearray

print(b"123" + bytearray(2))

print(b"" + bytearray(1)) # LHS is empty but can't be optimised
