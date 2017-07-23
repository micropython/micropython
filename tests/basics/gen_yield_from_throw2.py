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
