# to test keyword-only arguments

# simplest case
def f(*, a):
    print(a)

f(a=1)

# with 2 keyword-only args
def f(*, a, b):
    print(a, b)

f(a=1, b=2)
f(b=1, a=2)

# positional followed by bare star
def f(a, *, b, c):
    print(a, b, c)

f(1, b=3, c=4)
f(1, c=3, b=4)
f(1, **{'b':'3', 'c':4})

try:
    f(1)
except TypeError:
    print("TypeError")

try:
    f(1, b=2)
except TypeError:
    print("TypeError")

try:
    f(1, c=2)
except TypeError:
    print("TypeError")

# with **kw
def f(a, *, b, **kw):
    print(a, b, kw)

f(1, b=2)
f(1, b=2, c=3)

# with named star
def f(*a, b, c):
    print(a, b, c)

f(b=1, c=2)
f(c=1, b=2)

# with positional and named star
def f(a, *b, c):
    print(a, b, c)

f(1, c=2)
f(1, 2, c=3)
f(a=1, c=3)

# lambda as kw-only arg (exposes nested behaviour in compiler)
def f(*, x=lambda:1):
    return x()
print(f())
print(f(x=f))
print(f(x=lambda:2))
