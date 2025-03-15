try:
    from collections import deque
except ImportError:
    print("SKIP")
    raise SystemExit

d = deque((), 10)

d.append(1)
d.append(2)
d.append(3)

# Index slicing for reads is not supported in CPython
try:
    d[0:1]
except TypeError:
    print("TypeError")

# Index slicing for writes is not supported in CPython
try:
    d[0:1] = (-1, -2)
except TypeError:
    print("TypeError")

# Index slicing for writes is not supported in CPython
try:
    del d[0:1]
except TypeError:
    print("TypeError")
