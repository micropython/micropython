l = list(range(10000))
hex(1)
try:
    100000000 * l
except MemoryError:
    print('MemoryError')
print(len(l), l[0], l[-1])
