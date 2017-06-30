# test slices; only 2 argument version supported by MicroPython at the moment
x = list(range(10))

# Assignment
l = list(x)
l[1:3] = [10, 20]
print(l)
l = list(x)
l[1:3] = [10]
print(l)
l = list(x)
l[1:3] = []
print(l)
l = list(x)
del l[1:3]
print(l)

l = list(x)
l[:3] = [10, 20]
print(l)
l = list(x)
l[:3] = []
print(l)
l = list(x)
del l[:3]
print(l)

l = list(x)
l[:-3] = [10, 20]
print(l)
l = list(x)
l[:-3] = []
print(l)
l = list(x)
del l[:-3]
print(l)

# assign a tuple
l = [1, 2, 3]
l[0:1] = (10, 11, 12)
print(l)

# RHS of slice must be an iterable
try:
    [][0:1] = 123
except TypeError:
    print('TypeError')
