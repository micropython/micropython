try:
    from utimeq import utimeq
except ImportError:
    print("SKIP")
    raise SystemExit

h = utimeq(10)

# Check that for 2 same-key items, the queue is stable (pops items
# in the same order they were pushed). Unfortunately, this no longer
# holds for more same-key values, as the underlying heap structure
# is not stable itself.
h.push(100, 20, 0)
h.push(100, 10, 0)

res = [0, 0, 0]
h.pop(res)
assert res == [100, 20, 0]
h.pop(res)
assert res == [100, 10, 0]

print("OK")
