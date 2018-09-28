def gen():
    try:
        yield 1
    except ValueError:
        print("got ValueError from upstream!")
    yield "str1"
    raise TypeError

def gen2():
    print((yield from gen()))

g = gen2()
print(next(g))
print(g.throw(ValueError))
try:
    print(next(g))
except TypeError:
    print("got TypeError from downstream!")
