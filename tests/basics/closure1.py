# closures

def f(x):
    y = 2 * x
    def g(z):
        return y + z
    return g

print(f(1)(1))

x = f(2)
y = f(3)
print(x(1), x(2), x(3))
print(y(1), y(2), y(3))
print(x(1), x(2), x(3))
print(y(1), y(2), y(3))


# test various names in a dict passed as kwargs don't clash with the internal closure name
def f2(x):
    y = x

    def inner(**kwarg):
        return (y, kwarg)

    return inner


inner = f2(1)
print(inner())
print(inner(**{"": 2}))
print(inner(**{"*": 2}))
