# test errors from bad operations (unary, binary, etc)

# unsupported unary operators
try:
    ~None
except TypeError:
    print('TypeError')
try:
    ~''
except TypeError:
    print('TypeError')
try:
    ~[]
except TypeError:
    print('TypeError')

# unsupported binary operators
try:
    False in True
except TypeError:
    print('TypeError')
try:
    1 * {}
except TypeError:
    print('TypeError')
try:
    1 in 1
except TypeError:
    print('TypeError')

# unsupported subscription
try:
    1[0]
except TypeError:
    print('TypeError')
try:
    1[0] = 1
except TypeError:
    print('TypeError')
try:
    ''['']
except TypeError:
    print('TypeError')
try:
    'a'[0] = 1
except TypeError:
    print('TypeError')
try:
    del 1[0]
except TypeError:
    print('TypeError')

# not callable
try:
    1()
except TypeError:
    print('TypeError')

# not an iterator
try:
    next(1)
except TypeError:
    print('TypeError')

# must be an exception type
try:
    raise 1
except TypeError:
    print('TypeError')

# no such name in import
try:
    from sys import youcannotimportmebecauseidontexist
except ImportError:
    print('ImportError')
