# test passing named arg to closed-over function

def f():
    x = 1
    def g(z):
        print(x, z)
    return g

f()(z=42)
