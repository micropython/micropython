# test that the GC can trace nested objects

try:
    import gc
except ImportError:
    print("SKIP")
    raise SystemExit

# test a big shallow object pointing to many unique objects
lst = [[i] for i in range(200)]
gc.collect()
print(lst)

# test a deep object
lst = [
    [[[[(i, j, k, l)] for i in range(3)] for j in range(3)] for k in range(3)] for l in range(3)
]
gc.collect()
print(lst)
