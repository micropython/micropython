# test sys.getsizeof() function

import sys
try:
    sys.getsizeof
except AttributeError:
    print('SKIP')
    raise SystemExit

print(sys.getsizeof([1, 2]) >= 2)
print(sys.getsizeof({1: 2}) >= 2)

class A:
    pass
print(sys.getsizeof(A()) > 0)

# Only test deque if we have it
try:
    from ucollections import deque
    assert sys.getsizeof(deque((), 1)) > 0
except ImportError:
    pass
