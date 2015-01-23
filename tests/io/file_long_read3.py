f = open("io/data/bigfile2", "rb")
b = f.read(512)
print(len(b))
print(b)
