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

print(x + (10, 100, 10000))

# inplace add operator
x += (10, 11, 12)
print(x)

# construction of tuple from large iterator (tests implementation detail of MicroPython)
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
