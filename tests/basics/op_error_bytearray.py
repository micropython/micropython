# test errors from bad operations (unary, binary, etc)

# unsupported unary operators
try:
    ~bytearray()
except TypeError:
    print('TypeError')

# unsupported binary operators
try:
    bytearray() // 2
except TypeError:
    print('TypeError')

# object with buffer protocol needed on rhs
try:
    bytearray(1) + 1
except TypeError:
    print('TypeError')
