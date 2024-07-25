# Tests for deques with "check overflow" flag and other extensions
# wrt to CPython.
try:
    from collections import deque
except ImportError:
    print("SKIP")
    raise SystemExit

# CIRCUITPY-CHANGE: more functionality
# Initial sequence is supported
d = deque([1, 2, 3], 10)

# iteration over sequence
for x in d:
    print(x)

# Iterables larger than maxlen have the beginnings removed, also tests
# iteration through conversion to list
d = deque([1, 2, 3, 4, 5], 3)
print(list(d))

# Empty iterables are also supported
deque([], 10)

# Extending deques with iterables
d.extend([6, 7])
print(list(d))

# Accessing queue elements via index
d = deque((0, 1, 2, 3), 5)
print(d[0], d[1], d[-1])

# Writing queue elements via index
d[3] = 5
print(d[3])

# Accessing indices out of bounds raises IndexError
try:
    d[4]
except IndexError:
    print("IndexError")

try:
    d[4] = 0
except IndexError:
    print("IndexError")

# Removing elements with del is not supported, fall back on mp_obj_subscr() error message
try:
    del d[0]
except TypeError:
    print("TypeError")


# Only fixed-size deques are supported, so length arg is mandatory
try:
    deque(())
except TypeError:
    print("TypeError")

d = deque((), 2, True)

try:
    d.popleft()
except IndexError:
    print("IndexError")

try:
    d.pop()
except IndexError:
    print("IndexError")

print(d.append(1))
print(d.popleft())

d.append(2)
print(d.popleft())

d.append(3)
d.append(4)
print(d.popleft(), d.popleft())
try:
    d.popleft()
except IndexError as e:
    print(repr(e))

try:
    d.pop()
except IndexError as e:
    print(repr(e))

d.append(5)
d.append(6)
print(len(d))
try:
    d.append(7)
except IndexError as e:
    print(repr(e))

try:
    d.appendleft(8)
except IndexError as e:
    print(repr(e))

print(len(d))

print(d.popleft(), d.popleft())
print(len(d))
try:
    d.popleft()
except IndexError as e:
    print(repr(e))
