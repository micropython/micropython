# test bytearray + bytearray

b = bytearray(2)
b[0] = 1
b[1] = 2
print(b + bytearray(2))

# inplace add
b += bytearray(3)
print(b)

# extend
b.extend(bytearray(4))
print(b)

# this inplace add tests the code when the buffer doesn't need to be increased
b = bytearray()
b += b''
