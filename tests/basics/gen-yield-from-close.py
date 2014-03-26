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
# but raises StopIteration instead. This still should close chain properly.
def gen5():
    yield 1
    try:
        yield 2
    except GeneratorExit:
        print("leaf caught GeneratorExit and raised StopIteration instead")
        raise StopIteration(123)
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
