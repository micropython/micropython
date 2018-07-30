# test builtin type

print(type(int))

try:
    type()
except TypeError:
    print('TypeError')

try:
    type(1, 2)
except TypeError:
    print('TypeError')

# second arg should be a tuple
try:
    type('abc', None, None)
except TypeError:
    print('TypeError')

# third arg should be a dict
try:
    type('abc', (), None)
except TypeError:
    print('TypeError')

# elements of second arg (the bases) should be types
try:
    type('abc', (1,), {})
except TypeError:
    print('TypeError')
