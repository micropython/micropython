# test sys module

try:
    import usys as sys
except ImportError:
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
