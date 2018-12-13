# memoryview constructor with offset/size
try:
    memoryview(b"", 0, 0)
except:
    print("SKIP")
    raise SystemExit

buf = b"12345"

m = memoryview(buf, 1, 3)
print(list(m))

m = memoryview(buf, -1, 100)
print(list(m))

m = memoryview(buf, 200, -1)
print(list(m))
