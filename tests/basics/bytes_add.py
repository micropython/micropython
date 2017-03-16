# test bytes + other

print(b"123" + b"456")
print(b"123" + bytearray(2))

print(b"123" + b"") # RHS is empty, can be optimised
print(b"" + b"123") # LHS is empty, can be optimised
print(b"" + bytearray(1)) # LHS is empty but can't be optimised
