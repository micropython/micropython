from collections import namedtuple

T = namedtuple("Tup", "foo bar")
# CPython prints fully qualified name, what we don't bother to do so far
#print(T)
t = T(1, 2)
print(t)
print(t[0], t[1])
print(t.foo, t.bar)

print(len(t))
print(bool(t))
print(t + t)
print(t * 3)

print(isinstance(t, tuple))

try:
    t[0] = 200
except TypeError:
    print("TypeError")
try:
    t.bar = 200
except AttributeError:
    print("AttribiteError")

try:
    t = T(1)
except TypeError:
    print("TypeError")

try:
    t = T(1, 2, 3)
except TypeError:
    print("TypeError")

# Try comma field separator
T2 = namedtuple("TupComma", "foo,bar")
t = T2(1, 2)
print(t)
print(t.foo, t.bar)

# Try list of fields
# Not implemented so far
#T3 = namedtuple("TupComma", ["foo", "bar"])
#t = T3(1, 2)
