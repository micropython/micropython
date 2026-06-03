try:
    from collections import namedtuple
except ImportError:
    print("SKIP")
    raise SystemExit


class MyTuple(tuple):
    pass


N = namedtuple("N", ("a", "b"))


class MyNamedTuple(N):
    pass


t = (1, 2)
m = MyTuple((3, 4))
n = N(5, 6)
q = MyNamedTuple(7, 8)

print(t + t)
print(t + m)
print(t + n)
print(t + q)

print(m + t)
print(m + m)
print(m + n)
print(m + q)

print(n + t)
print(n + m)
print(n + n)
print(n + q)

print(q + t)
print(q + m)
print(q + n)
print(q + q)

print(t < t)
print(t < m)
print(t < n)
print(t < q)

print(m < t)
print(m < m)
print(m < n)
print(m < q)

print(n < t)
print(n < m)
print(n < n)
print(n < q)

print(q < t)
print(q < m)
print(q < n)
print(q < q)
