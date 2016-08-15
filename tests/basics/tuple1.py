# basic tuple functionality
x = (1, 2, 3 * 4)
print(x)
try:
    x[0] = 4
except TypeError:
    print("TypeError")
print(x)
try:
    x.append(5)
except AttributeError:
    print("AttributeError")

print(x[1:])
print(x[:-1])
print(x[2:3])

print(x + (10, 100, 10000))

# construction of tuple from large iterator (tests implementation detail of uPy)
print(tuple(range(20)))

# unsupported unary operation
try:
    +()
except TypeError:
    print('TypeError')

# unsupported type on RHS of add
try:
    () + None
except TypeError:
    print('TypeError')
