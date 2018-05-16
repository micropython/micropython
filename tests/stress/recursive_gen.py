# test deeply recursive generators

# simple "yield from" recursion
def gen():
    yield from gen()
try:
    list(gen())
except RuntimeError:
    print('RuntimeError')

# recursion via an iterator over a generator
def gen2():
    for x in gen2():
        yield x
try:
    next(gen2())
except RuntimeError:
    print('RuntimeError')
