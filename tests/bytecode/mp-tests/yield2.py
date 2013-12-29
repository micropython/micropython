def f():
    yield from a
    yield from (a, b)
    yield from f(a)
