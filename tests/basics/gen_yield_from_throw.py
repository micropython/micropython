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
    g3.throw(KeyError)
except KeyError:
    print('got KeyError from downstream!')

# case where a thrown exception is caught and stops the generator
def gen4():
    try:
        yield 1
        yield 2
    except:
        pass
g4 = gen4()
print(next(g4))
try:
    g4.throw(ValueError)
except StopIteration:
    print('got StopIteration')
