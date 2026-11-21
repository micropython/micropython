# copying a large dictionary

try:
    a = {i: 2 * i for i in range(1000)}
    b = a.copy()
except MemoryError:
    print("SKIP")
    raise SystemExit

for i in range(1000):
    print(i, b[i])
print(len(b))
