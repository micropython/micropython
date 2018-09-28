# case where generator doesn't intercept the thrown/injected exception
def gen():
    yield 123
    yield 456
        
g = gen()
print(next(g))
try:
    g.throw(KeyError)
except KeyError:
    print('got KeyError from downstream!')

# case where a thrown exception is caught and stops the generator
def gen():
    try:
        yield 1
        yield 2
    except:
        pass
g = gen()
print(next(g))
try:
    g.throw(ValueError)
except StopIteration:
    print('got StopIteration')

# generator ignores a thrown GeneratorExit (this is allowed)
def gen():
    try:
        yield 123
    except GeneratorExit:
        print('GeneratorExit')
    yield 456
        
# thrown a class
g = gen()
print(next(g))
print(g.throw(GeneratorExit))

# thrown an instance
g = gen()
print(next(g))
print(g.throw(GeneratorExit()))
