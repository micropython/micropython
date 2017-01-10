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

# case where generator doesn't intercept the thrown/injected exception
def gen3():
    yield 123
    yield 456
        
g3 = gen3()
print(next(g3))
try:
    g3.throw(StopIteration)
except StopIteration:
    print('got StopIteration from downstream!')
