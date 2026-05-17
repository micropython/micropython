try:
    import micropython
    micropython.heap_lock
except:
    print("SKIP")
    raise SystemExit

# Skip this test on "REPR B" where (1<<29 + 1) is not a small int.
k = 1 << 29
micropython.heap_lock()
try:
    k = k + 1
except MemoryError:
    print("SKIP")
    raise SystemExit
finally:
    micropython.heap_unlock()

# All less than small int max.
for d in (0, 27, 1<<29, -1861, -(1<<29)):
    i = 1<<70
    print(i)
    j = (1<<70) + d
    print(j)
    # k should now be a small int.
    k = j - i
    print(k)

    # Now verify that working with k doesn't allocate (i.e. it's a small int).
    micropython.heap_lock()
    print(k + 20)
    print(k // 20)
    micropython.heap_unlock()
