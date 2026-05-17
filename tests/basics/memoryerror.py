# test out-of-memory with malloc
l = list(range(1000))
try:
    1000000000 * l
except MemoryError:
    print('MemoryError')
print(len(l), l[0], l[-1])

# test out-of-memory with realloc
try:
    [].extend(range(1000000000))
except MemoryError:
    print('MemoryError')
