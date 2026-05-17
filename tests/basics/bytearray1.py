print(bytearray(4))
a = bytearray([1, 2, 200])
print(type(a))
print(a[0], a[2])
print(a[-1])
print(a)
a[2] = 255
print(a[-1])
a.append(10)
print(len(a))

s = 0
for i in a:
    s += i
print(s)

print(a[1:])
print(a[:-1])
print(a[2:3])

print(str(bytearray(b"123"), "utf-8"))

# Comparisons
print(bytearray([1]) == bytearray([1]))
print(bytearray([1]) == bytearray([2]))
print(bytearray([1]) == b"1")
print(b"1" == bytearray([1]))
print(bytearray() == bytearray())

b1 = bytearray([1, 2, 3])
b2 = bytearray([1, 2, 3])
b3 = bytearray([1, 3])
print(b1 == b2)
print(b2 != b3)
print(b1 <= b2)
print(b1 <= b3)
print(b1 < b3)
print(b1 >= b2)
print(b3 >= b2)
print(b3 > b2)
print(b1 != b2)
print(b2 == b3)
print(b1 > b2)
print(b1 > b3)
print(b1 >= b3)
print(b1 < b2)
print(b3 < b2)
print(b3 <= b2)

# comparison with other type should return False
print(bytearray() == 1)

# TODO: other comparisons

# __contains__
b = bytearray(b"\0foo\0")
print(b"foo" in b)
print(b"foo\x01" in b)
print(b"" in b)
