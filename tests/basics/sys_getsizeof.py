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
