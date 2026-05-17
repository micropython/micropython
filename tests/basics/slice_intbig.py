# test slicing when arguments are bignums

print(list(range(10))[(1<<66)>>65:])
print(list(range(10))[:(1<<66)>>65])
print(list(range(10))[::(1<<66)>>65])
