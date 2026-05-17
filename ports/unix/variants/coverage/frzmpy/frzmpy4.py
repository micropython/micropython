# Test importing frozen functions.

# A simple bytecode function with no children.
def f():
    return 1


print(__name__, f())


# A simple bytecode generator with no children.
def g():
    yield 2


print(__name__, next(g()))
