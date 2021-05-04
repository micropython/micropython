# test bytes + other

print(b"123" + b"456")

print(b"123" + b"") # RHS is empty, can be optimised
print(b"" + b"123") # LHS is empty, can be optimised
