# test out-of-memory with malloc
l = list(range(1000))
try:
    1000000000 * l
except (MemoryError, OverflowError):
    # On targets where size_t is 32 bits, 1000 * 1000000000 elements
    # overflows the allocation size check and raises OverflowError
    # before an allocation is even attempted, instead of the allocation
    # itself failing with MemoryError.
    print('MemoryError')
print(len(l), l[0], l[-1])

# test out-of-memory with realloc
try:
    [].extend(range(1000000000))
except MemoryError:
    print('MemoryError')
