# Test MicroPython-specific features of collections.deque.

try:
    from collections import deque
except ImportError:
    print("SKIP")
    raise SystemExit


# Only fixed-size deques are supported, so length arg is mandatory.
try:
    deque(())
except TypeError:
    print("TypeError")

# Test third argument: flags when True means check for under/overflow
d = deque((), 2, True)

try:
    d.popleft()
except IndexError:
    print("IndexError")

try:
    d.pop()
except IndexError:
    print("IndexError")

# Removing elements with del is not supported, fallback to
# mp_obj_subscr() error message.
try:
    del d[0]
except TypeError:
    print("TypeError")

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
