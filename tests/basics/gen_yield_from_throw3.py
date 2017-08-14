# yield-from a user-defined generator with a throw() method

class Iter:
    def __iter__(self):
        return self

    def __next__(self):
        return 1

    def throw(self, x):
        print('throw', x)
        return 456

def gen():
    yield from Iter()

# calling close() should not call throw()
g = gen()
print(next(g))
g.close()

# can throw a non-exception object
g = gen()
print(next(g))
print(g.throw(123))

# throwing an exception class just injects that class
g = gen()
print(next(g))
print(g.throw(ZeroDivisionError))
