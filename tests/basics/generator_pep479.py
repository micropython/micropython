# tests for correct PEP479 behaviour (introduced in Python 3.5)

# basic case: StopIteration is converted into a RuntimeError
def gen():
    yield 1
    raise StopIteration
g = gen()
print(next(g))
try:
    next(g)
except RuntimeError:
    print('RuntimeError')

# trying to continue a failed generator now raises StopIteration
try:
    next(g)
except StopIteration:
    print('StopIteration')

# throwing a StopIteration which is uncaught will be converted into a RuntimeError
def gen():
    yield 1
    yield 2
g = gen()
print(next(g))
try:
    g.throw(StopIteration)
except RuntimeError:
    print('RuntimeError')
