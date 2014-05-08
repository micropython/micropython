# test constant optimisation

X = const(123)
Y = const(X + 456)

print(X, Y + 1)

def f():
    print(X, Y + 1)

f()
