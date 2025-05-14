# test sys module

import sys
print(sys.__name__)
print(type(sys.path))
print(type(sys.argv))
print(sys.byteorder in ('little', 'big'))

try:
    print(sys.maxsize > 100)
except AttributeError:
    # Effectively skip subtests
    print(True)

try:
    print(sys.implementation.name in ('cpython', 'micropython'))
except AttributeError:
    # Effectively skip subtests
    print(True)

if hasattr(sys.implementation, '_mpy'):
    print(type(sys.implementation._mpy))
else:
    # Effectively skip subtests
    print(int)

if hasattr(sys.implementation, '_build'):
    print(type(sys.implementation._build))
else:
    # Effectively skip subtests
    print(str)

try:
    print(sys.intern('micropython') == 'micropython')
    has_intern = True
except AttributeError:
    has_intern = False
    print(True)

if has_intern:
    try:
        print(sys.intern(0))
    except TypeError:
        print(True)
else:
    print(True)
