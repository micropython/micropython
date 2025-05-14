try:
    bytearray()[:] = bytearray()
except TypeError:
    print("SKIP")
    raise SystemExit

# test slices; only 2 argument version supported by MicroPython at the moment
x = bytearray(range(10))

# Assignment
l = bytearray(x)
l[1:3] = bytearray([10, 20])
print(l)
l = bytearray(x)
l[1:3] = bytearray([10])
print(l)
l = bytearray(x)
l[1:3] = bytearray()
print(l)
l = bytearray(x)
# del l[1:3]
print(l)

l = bytearray(x)
l[:3] = bytearray([10, 20])
print(l)
l = bytearray(x)
l[:3] = bytearray()
print(l)
l = bytearray(x)
# del l[:3]
print(l)

l = bytearray(x)
l[:-3] = bytearray([10, 20])
print(l)
l = bytearray(x)
l[:-3] = bytearray()
print(l)
l = bytearray(x)
# del l[:-3]
print(l)

# slice assignment that extends the array
b = bytearray(2)
b[2:] = bytearray(10)
print(b)

b = bytearray(10)
b[:-1] = bytearray(500)
print(len(b), b[0], b[-1])

# extension with self on RHS
b = bytearray(x)
b[4:] = b
print(b)

# Assignment of bytes to array slice
b = bytearray(2)
b[1:1] = b"12345"
print(b)

# Growth of bytearray via slice extension
b = bytearray(b"12345678")
b.append(57)  # expand and add a bit of unused space at end of the bytearray
for i in range(400):
    b[-1:] = b"ab"  # grow slowly into the unused space
print(len(b), b)

# Growth of bytearray via slice extension from itself
b = bytearray(b"1234567")
for i in range(3):
    b[-1:] = b
print(len(b), b)
