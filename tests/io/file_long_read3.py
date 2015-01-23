f = open("io/data/bigfile1", "rb")
b = f.read(512)
print(len(b))
print(b)
