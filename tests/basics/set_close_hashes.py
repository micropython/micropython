# test sets whose str and bytes elements have hashes that are close together
# requires: set

n = 1000

# skip on targets without the memory for this test: twice n elements and sets
# of them, to leave room for the heap to fragment
try:
    probe = []
    for j in range(2):
        keys = ["name-" + str(10000 + i) for i in range(n)]
        probe.append((keys, set(keys)))
except MemoryError:
    print("SKIP")
    raise SystemExit
probe = keys = None

for make_key in (
    lambda i: "name-" + str(10000 + i),
    lambda i: bytes("name-" + str(10000 + i), "ascii"),
):
    keys = [make_key(i) for i in range(n)]

    # a set of them holds every element, and reports missing ones
    s = set(keys)
    print(len(s))
    ok = True
    for i in range(n):
        if keys[i] not in s:
            ok = False
        if make_key(i + n) in s:
            ok = False
    print(ok)

    # remove every other element, then add them back
    for i in range(0, n, 2):
        s.remove(keys[i])
    print(len(s), keys[0] in s, keys[1] in s)
    for i in range(0, n, 2):
        s.add(keys[i])
    ok = len(s) == n
    for k in keys:
        if k not in s:
            ok = False
    print(len(s), ok)

    # iterating over the set gives every element once
    print(sorted(s) == sorted(keys))

    # free them before the next pass, for targets with a small heap
    keys = s = None
