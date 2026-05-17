# test return statement

def f():
    return
print(f())

def g():
    return 1
print(g())

def f(x):
    return 1 if x else 2
print(f(0), f(1))
