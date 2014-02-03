print(id(1) == id(2))
print(id(None) == id(None))
# This can't be true per Python semantics, just CPython implementation detail
#print(id([]) == id([]))

l = [1, 2]
print(id(l) == id(l))

f = lambda:None
print(id(f) == id(f))
