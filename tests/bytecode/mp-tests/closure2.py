# test closing over an argument

def f(x):
    y = 2 * x
    def g(z):
        return x + y + z
    return g
