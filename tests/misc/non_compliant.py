# tests for things that are not implemented, or have non-compliant behaviour

import array

# array deletion not implemented
try:
    a = array.array('b', (1, 2, 3))
    del a[1]
except TypeError:
    print('TypeError')

# slice with step!=1 not implemented
try:
    a = array.array('b', (1, 2, 3))
    print(a[3:2:2])
except NotImplementedError:
    print('NotImplementedError')

# should raise type error
try:
    print(set('12') >= '1')
except TypeError:
    print('TypeError')

# should raise type error
try:
    print(set('12') <= '123')
except TypeError:
    print('TypeError')

# uPy raises TypeError, shold be ValueError
try:
    '%c' % b'\x01\x02'
except (TypeError, ValueError):
    print('TypeError, ValueError')

# attributes/subscr not implemented
try:
    print('{a[0]}'.format(a=[1, 2]))
except NotImplementedError:
    print('NotImplementedError')

# str(...) with keywords not implemented
try:
    str(b'abc', encoding='utf8')
except NotImplementedError:
    print('NotImplementedError')

# str.rsplit(None, n) not implemented
try:
    'a a a'.rsplit(None, 1)
except NotImplementedError:
    print('NotImplementedError')

# str.endswith(s, start) not implemented
try:
    'abc'.endswith('c', 1)
except NotImplementedError:
    print('NotImplementedError')

# bytes(...) with keywords not implemented
try:
    bytes('abc', encoding='utf8')
except NotImplementedError:
    print('NotImplementedError')

# bytes subscr with step!=1 not implemented
try:
    b'123'[0:3:2]
except NotImplementedError:
    print('NotImplementedError')

