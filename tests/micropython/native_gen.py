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


# catching an exception from .throw()
@micropython.native
def gen3():
    try:
        yield 1
        yield 2
    except Exception as er:
        print("caught", repr(er))
        yield 3


g = gen3()
print(next(g))
print(g.throw(ValueError(42)))


# responding to .close()
@micropython.native
def gen4():
    try:
        yield 1
    except:
        print("raising GeneratorExit")
        raise GeneratorExit


g = gen4()
print(next(g))
print(g.close())
