# basic list functionality
x = [1, 2, 3 * 4]
print(x)
x[0] = 4
print(x)
x[1] += -4
print(x)
x.append(5)
print(x)
f = x.append
f(4)
print(x)

x.extend([100, 200])
print(x)
x.extend(range(3))
print(x)

x += [2, 1]
print(x)

# unsupported type on RHS of add
try:
    [] + None
except TypeError:
    print('TypeError')
