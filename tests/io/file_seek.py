f = open("io/data/file1", "rb")
print(f.seek(6))
print(f.read(5))
print(f.tell())

print(f.seek(0, 1))
print(f.read(4))
print(f.tell())

print(f.seek(-6, 2))
print(f.read(20))
print(f.tell())

print(f.seek(0, 0))
print(f.read(5))
print(f.tell())

f.close()

# test text mode
f = open("io/data/file1", "rt")
print(f.seek(6))
print(f.read(5))
print(f.tell())
f.close()
