# test positional-only arguments (PEP 570)


def test_kw(f, *args, **kwargs):
    try:
        print(f(*args, **kwargs))
    except TypeError:
        print("TypeError")


# simplest case
def f(a, /):
    return a


print(f(1))
test_kw(f, a=1)
test_kw(f, **{"": 1})
test_kw(f, **{"*": 1})


# simplest case with trailing comma
def f(
    a,
    /,
):
    return a


print(f(1))


# positional-only followed by normal arg
def f(a, /, b):
    return a, b


print(f(1, 2))
print(f(1, b=2))
test_kw(f, a=1, b=2)
test_kw(f, b=2)


# with everything
def f(a, b=2, /, c=3, *args, d, e=5, **kwargs):
    return a, b, c, args, d, e, sorted(kwargs.items())


print(f(1, d=4))
print(f(1, 2, 3, 4, 5, d=6, e=7, a=8, b=9, z=10))
print(f(1, c=3, d=4, b=20))
test_kw(f, d=4, a=1)


# positional-only args mixed with closed-over variables
def f(x, /, y):
    def g(x2, /, y2):
        return x + y + x2 + y2

    return g


print(f(1, 2)(3, 4))
print(f(1, 2)(3, y2=4))
test_kw(f(1, 2), x2=3, y2=4)

# lambdas
f = lambda a, /, b: (a, b)
print(f(1, 2))
print(f(1, b=2))
test_kw(f, a=1, b=2)
