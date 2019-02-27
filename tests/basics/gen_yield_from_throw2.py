# outer generator ignores a thrown GeneratorExit (this is allowed)

def gen():
    try:
        yield 123
    except GeneratorExit:
        print('GeneratorExit')

def gen2():
    try:
        yield from gen()
    except GeneratorExit:
        print('GeneratorExit outer')
    yield 789

# thrown a class
g = gen2()
print(next(g))
print(g.throw(GeneratorExit))

# thrown an instance
g = gen2()
print(next(g))
print(g.throw(GeneratorExit()))
