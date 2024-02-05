b = bytearray(30)
f = open("data/file1", "rb")
print(f.readinto(b))
print(b)
f = open("data/file2", "rb")
print(f.readinto(b))
print(b)

# readinto() on writable file
f = open("data/file1", "ab")
try:
    f.readinto(bytearray(4))
except OSError:
    print("OSError")
