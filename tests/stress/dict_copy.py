# copying a large dictionary

try:
    a = {i: 2 * i for i in range(1000)}
except MemoryError:
    print("SKIP")
    raise SystemExit

b = a.copy()
for i in range(1000):
    print(i, b[i])
print(len(b))
