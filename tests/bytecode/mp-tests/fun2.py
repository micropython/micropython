def f(*, b):
    return b

def f(a, *, b):
    return a + b

def f(a, *, b, c):
    return a + b + c

def f(a, *, b=c):
    return a + b

def f(a, *, b=c, c):
    return a + b + c

def f(a, *, b=c, c=d):
    return a + b + c

def f(a, *, b=c, c, d=e):
    return a + b + c + d

def f(a=None, *, b=None):
    return a + b
