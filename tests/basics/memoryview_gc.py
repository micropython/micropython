# test memoryview retains pointer to original object/buffer

b = bytearray(10)
m = memoryview(b)[1:]
for i in range(len(m)):
    m[i] = i

# reclaim b, but hopefully not the buffer
b = None
import gc
gc.collect()

# allocate lots of memory
for i in range(100000):
    [42, 42, 42, 42]

# check that the memoryview is still what we want
print(list(m))
