# test when we subclass a type with the buffer protocol

class my_bytes(bytes):
    pass

b1 = my_bytes([0, 1])
b2 = my_bytes([2, 3])
b3 = bytes([4, 5])

# addition will use the buffer protocol on the RHS
print(b1 + b2)
print(b1 + b3)
print(b3 + b1)

# bytes construction will use the buffer protocol
print(bytes(b1))
