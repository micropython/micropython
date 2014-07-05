def gen():
    yield 1
    return 42

g = gen()
print(next(g))
try:
    print(next(g))
except StopIteration as e:
    print(repr(e))
