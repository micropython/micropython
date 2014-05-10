# test slices; only 2 argument version supported by Micro Python at the moment
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
