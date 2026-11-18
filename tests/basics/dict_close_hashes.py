# test dicts whose str and bytes keys have hashes that are close together

n = 1000

# skip on targets without the memory for this test: twice n keys and dicts of
# them, to leave room for the heap to fragment
try:
    probe = []
    for j in range(2):
        keys = ["name-" + str(10000 + i) for i in range(n)]
        d = {}
        for k in keys:
            d[k] = None
        probe.append((keys, d))
except MemoryError:
    print("SKIP")
    raise SystemExit
probe = keys = d = None

for make_key in (
    lambda i: "name-" + str(10000 + i),
    lambda i: bytes("name-" + str(10000 + i), "ascii"),
):
    keys = [make_key(i) for i in range(n)]

    # a dict of them holds and finds every key, and reports missing ones
    d = {}
    for i in range(n):
        d[keys[i]] = i
    print(len(d))
    ok = True
    for i in range(n):
        if d[keys[i]] != i:
            ok = False
        if make_key(i + n) in d:
            ok = False
    print(ok)

    # delete every other key, then look the rest up and add the deleted ones back
    for i in range(0, n, 2):
        del d[keys[i]]
    print(len(d), keys[0] in d, keys[1] in d)
    ok = True
    for i in range(1, n, 2):
        if d[keys[i]] != i:
            ok = False
    for i in range(0, n, 2):
        d[keys[i]] = -i
    for i in range(n):
        if d[keys[i]] != (-i if i % 2 == 0 else i):
            ok = False
    print(len(d), ok)

    # iterating over the dict gives every key once
    count = 0
    total = 0
    for k in d:
        count += 1
        total += d[k]
    print(count, total)

    # free them before the next pass, for targets with a small heap
    keys = d = None

# str keys that are interned (qstrs) and str keys that are not find each other
d = {}
for i in range(100):
    d["x" + str(i)] = i
ok = True
for i in range(100):
    if d["x" + str(i)] != i:
        ok = False
print(ok, d["x0"], d["x99"])
