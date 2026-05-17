# create a large dictionary

d = {}
x = 1
while x < 1000:
    try:
        d[x] = x
    except MemoryError:
        print("SKIP")
        raise SystemExit
    x += 1
print(d[500])
