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

# this user-defined generator doesn't have a throw() method
class Iter2:
    def __iter__(self):
        return self

    def __next__(self):
        return 1

def gen2():
    yield from Iter2()

# the thrown ValueError is not intercepted by the user class
g = gen2()
print(next(g))
try:
    g.throw(ValueError)
except:
    print('ValueError')

# the thrown 123 is not an exception so raises a TypeError
g = gen2()
print(next(g))
try:
    g.throw(123)
except TypeError:
    print('TypeError')
