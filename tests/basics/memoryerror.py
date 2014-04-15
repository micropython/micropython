l = list(range(10000))
try:
    hex(1)
except NameError:
    print("NameError")
try:
    100000000 * l
except MemoryError:
    print('MemoryError')
print(len(l), l[0], l[-1])
