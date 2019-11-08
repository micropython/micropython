# test for native generators

# simple generator with yield and return
@micropython.native
def gen1(x):
    yield x
    yield x + 1
    return x + 2
g = gen1(3)
print(next(g))
print(next(g))
try:
    next(g)
except StopIteration as e:
    print(e.args[0])

# using yield from
@micropython.native
def gen2(x):
    yield from range(x)
print(list(gen2(3)))
