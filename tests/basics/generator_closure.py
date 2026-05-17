# a generator that closes over outer variables
def f():
    x = 1 # closed over by g
    def g():
        yield x
        yield x + 1
    return g()
for i in f():
    print(i)

# a generator that has its variables closed over
def f():
    x = 1 # closed over by g
    def g():
        return x + 1
    yield g()
    x = 2
    yield g()
for i in f():
    print(i)

# using comprehensions, the inner generator closes over y
generator_of_generators = (((x, y) for x in range(2)) for y in range(3))
for i in generator_of_generators:
    for j in i:
        print(j)

# test that printing of closed-over generators doesn't lead to segfaults
def genc():
    foo = 1
    repr(lambda: (yield foo))
genc()
