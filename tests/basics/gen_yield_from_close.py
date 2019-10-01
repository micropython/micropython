def gen():
    yield 1
    yield 2
    yield 3
    yield 4

def gen2():
    yield -1
    print((yield from gen()))
    yield 10
    yield 11

g = gen2()
print(next(g))
print(next(g))
g.close()
try:
    print(next(g))
except StopIteration:
    print("StopIteration")


# Now variation of same test, but with leaf generator
# swallowing GeneratorExit exception - its upstream gen
# generator should still receive one.
def gen3():
    yield 1
    try:
        yield 2
    except GeneratorExit:
        print("leaf caught GeneratorExit and swallowed it")
        return
    yield 3
    yield 4

def gen4():
    yield -1
    try:
        print((yield from gen3()))
    except GeneratorExit:
        print("delegating caught GeneratorExit")
        raise
    yield 10
    yield 11

g = gen4()
print(next(g))
print(next(g))
print(next(g))
g.close()
try:
    print(next(g))
except StopIteration:
    print("StopIteration")


# Yet another variation - leaf generator gets GeneratorExit,
# and reraises a new GeneratorExit. This still should close chain properly.
def gen5():
    yield 1
    try:
        yield 2
    except GeneratorExit:
        print("leaf caught GeneratorExit and reraised GeneratorExit")
        raise GeneratorExit(123)
    yield 3
    yield 4

def gen6():
    yield -1
    try:
        print((yield from gen5()))
    except GeneratorExit:
        print("delegating caught GeneratorExit")
        raise
    yield 10
    yield 11

g = gen6()
print(next(g))
print(next(g))
print(next(g))
g.close()
try:
    print(next(g))
except StopIteration:
    print("StopIteration")

# case where generator ignores the close request and yields instead
def gen7():
    try:
        yield 123
    except GeneratorExit:
        yield 456

g = gen7()
print(next(g))
try:
    g.close()
except RuntimeError:
    print('RuntimeError')

# case where close is propagated up to a built-in iterator
def gen8():
    g = range(2)
    yield from g
g = gen8()
print(next(g))
g.close()

# case with a user-defined close method
class Iter:
    def __iter__(self):
        return self
    def __next__(self):
        return 1
    def close(self):
        print('close')
def gen9():
    yield from Iter()
g = gen9()
print(next(g))
g.close()
