# test native emitter can handle closures correctly

# basic closure
@micropython.native
def f():
    x = 1
    @micropython.native
    def g():
        nonlocal x
        return x
    return g
print(f()())

# closing over an argument
@micropython.native
def f(x):
    @micropython.native
    def g():
        nonlocal x
        return x
    return g
print(f(2)())

# closing over an argument and a normal local
@micropython.native
def f(x):
    y = 2 * x
    @micropython.native
    def g(z):
        return x + y + z
    return g
print(f(2)(3))
