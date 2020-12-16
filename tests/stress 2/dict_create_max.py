# The aim with this test is to hit the maximum resize/rehash size of a dict,
# where there are no more primes in the table of growing allocation sizes.
# This value is 54907 items.

d = {}
try:
    for i in range(54908):
        d[i] = i
except MemoryError:
    pass

# Just check the dict still has the first value
print(d[0])
