# yielding from an already executing generator is not allowed

def f():
    yield 1
    # g here is already executing so this will raise an exception
    yield from g

g = f()

print(next(g))

try:
    next(g)
except ValueError:
    print('ValueError')
