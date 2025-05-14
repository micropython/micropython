try:
    from collections import deque
except ImportError:
    print("SKIP")
    raise SystemExit


d = deque((), 2)
print(len(d))
print(bool(d))

try:
    d.popleft()
except IndexError:
    print("IndexError")

print(d.append(1))
print(len(d))
print(bool(d))
print(d.popleft())
print(len(d))

d.append(2)
print(d.popleft())

d.append(3)
d.append(4)
print(len(d))
print(d.popleft(), d.popleft())
try:
    d.popleft()
except IndexError:
    print("IndexError")

d.append(5)
d.append(6)
d.append(7)
print(len(d))
print(d.popleft(), d.popleft())
print(len(d))
try:
    d.popleft()
except IndexError:
    print("IndexError")

# Case where get index wraps around when appending to full deque
d = deque((), 2)
d.append(1)
d.append(2)
d.append(3)
d.append(4)
d.append(5)
print(d.popleft(), d.popleft())

# Negative maxlen is not allowed
try:
    deque((), -1)
except ValueError:
    print("ValueError")

# Unsupported unary op
try:
    ~d
except TypeError:
    print("TypeError")


# Same tests, but now with pop() and appendleft()

d = deque((), 2)
print(len(d))
print(bool(d))

try:
    d.popleft()
except IndexError:
    print("IndexError")

print(d.append(1))
print(len(d))
print(bool(d))
print(d.popleft())
print(len(d))

d.append(2)
print(d.popleft())

d.append(3)
d.append(4)
print(len(d))
print(d.popleft(), d.popleft())
try:
    d.popleft()
except IndexError:
    print("IndexError")

d.append(5)
d.append(6)
d.append(7)
print(len(d))
print(d.popleft(), d.popleft())
print(len(d))
try:
    d.popleft()
except IndexError:
    print("IndexError")

d = deque((), 2)
d.appendleft(1)
d.appendleft(2)
d.appendleft(3)
d.appendleft(4)
d.appendleft(5)
print(d.pop(), d.pop())