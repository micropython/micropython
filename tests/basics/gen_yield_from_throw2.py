# uPy differs from CPython for this test
# generator ignored GeneratorExit
def gen():
    try:
        yield 123
    except GeneratorExit:
        print('GeneratorExit')
    yield 456
        
g = gen()
print(next(g))
try:
    g.throw(GeneratorExit)
except RuntimeError:
    print('RuntimeError')
