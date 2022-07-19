# cmdline: -v -v
# test printing of all bytecodes
# fmt: off

def f():
    # constants
    a = None + False + True
    a = 0
    a = 1000
    a = -1000

    # constructing data
    a = 1
    b = (1, 2)
    c = [1, 2]
    d = {1, 2}
    e = {}
    f = {1:2}
    g = 'a'
    h = b'a'

    # unary/binary ops
    i = 1
    j = 2
    k = a + b
    l = -a
    m = not a
    m = a == b == c
    m = not (a == b and b == c)

    # attributes
    n = b.c
    b.c = n

    # subscript
    p = b[0]
    b[0] = p
    b[0] += p

    # slice
    a = b[::]

    # sequence unpacking
    a, b = c
    a, *a = a

    # tuple swapping
    a, b = b, a
    a, b, c = c, b, a

    # del fast
    del a

    # globals
    global gl
    gl = a
    del gl

    # comprehensions
    a = (b for c in d if e)
    a = [b for c in d if e]
    a = {b:b for c in d if e}

    # function calls
    a()
    a(1)
    a(b=1)
    a(*b)

    # method calls
    a.b()
    a.b(1)
    a.b(c=1)
    a.b(*c)

    # jumps
    if a:
        x
    else:
        y
    while a:
        b
    while not a:
        b
    a = a or a

    # for loop
    for a in b:
        c

    # exceptions
    try:
        while a:
            break
    except:
        b
    finally:
        c
    while a:
        try:
            break
        except:
            pass

    # with
    with a:
        b

    # closed over variables
    x = 1
    def closure():
        nonlocal x; a = x + 1
        x = 1
        del x

    # import
    import a
    from a import b
    #from sys import * # tested at module scope

    # raise
    if a: raise
    if a: raise 1

    # return
    if a: return
    if a: return 1

# function with lots of locals
def f():
    l1 = l2 = l3 = l4 = l5 = l6 = l7 = l8 = l9 = l10 = 1
    m1 = m2 = m3 = m4 = m5 = m6 = m7 = m8 = m9 = m10 = 2
    l10 + m10

# functions with default args
def f(a=1):
    pass

    def f(b=2):
        return b + a

# function which yields
def f():
    yield
    yield 1
    yield from 1

# class
class Class:
    pass

# delete name
del Class

# load super method
def f(self):
    super().f()

# import * (needs to be in module scope)
from sys import *
