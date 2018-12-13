# memoryview.init()
try:
    memoryview.init
except:
    print("SKIP")
    raise SystemExit

buf = b"12345"

m = memoryview(buf, 1, 3)
print(list(m))

m.init(buf, -1, 100)
print(list(m))

m.init(buf, 200, -1)
print(list(m))
