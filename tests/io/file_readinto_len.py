b = bytearray(30)
f = open("data/file1", "rb")
# 2nd arg (length to read) is extension to CPython
print(f.readinto(b, 8))
print(b)

b = bytearray(4)
f = open("data/file1", "rb")
print(f.readinto(b, 8))
print(b)
