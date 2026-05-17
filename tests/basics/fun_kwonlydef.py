# test function args, keyword only with default value

# a single arg with a default
def f1(*, a=1):
    print(a)
f1()
f1(a=2)

# 1 arg default, 1 not
def f2(*, a=1, b):
    print(a, b)
f2(b=2)
f2(a=2, b=3)

# 1 positional, 1 arg default, 1 not
def f3(a, *, b=2, c):
    print(a, b, c)
f3(1, c=3)
f3(1, b=3, c=4)
f3(1, **{'c':3})
f3(1, **{'b':'3', 'c':4})

# many args, not all with defaults
def f4(*, a=1, b, c=3, d, e=5, f):
    print(a, b, c, d, e, f)
f4(b=2, d=4, f=6)
f4(a=11, b=2, d=4, f=6)
f4(a=11, b=2, c=33, d=4, e=55, f=6)
f4(f=6, e=55, d=4, c=33, b=2, a=11)

# positional with default, then keyword only
def f5(a, b=4, *c, d=8):
    print(a, b, c, d)
f5(1)
f5(1, d=9)
f5(1, b=44, d=9)
