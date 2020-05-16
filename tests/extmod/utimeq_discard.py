import sys

try:
    from utimeq import utimeq
except ImportError:
    print("SKIP")
    raise SystemExit

h = utimeq(10)

# discard on empty queue should not fail
h.discard("a")

h.push(10, "a", 0)
h.push(10, "b", 0)
h.push(20, "c", 0)
h.push(20, "a", 0)
h.push(30, "d", 0)
h.push(30, "a", 0)
h.push(30, "e", 0)

h.discard("a")

res = [0, 0, 0]
h.pop(res)
assert res == [10, "b", 0]
h.pop(res)
assert res == [20, "c", 0]
h.pop(res)
assert res == [30, "d", 0]
h.pop(res)
assert res == [30, "e", 0]

print("OK")
