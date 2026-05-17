# test scoping rules

# explicit global variable
a = 1
def f():
    global a
    global a, a # should be able to redefine as global
    a = 2
f()
print(a)

# explicit nonlocal variable
def f():
    a = 1
    def g():
        nonlocal a
        nonlocal a, a # should be able to redefine as nonlocal
        a = 2
    g()
    return a
print(f())

# nonlocal at inner-inner level (h)
def f():
    x = 1
    def g():
        def h():
            nonlocal x
            return x
        return h
    return g
print(f()()())

# nonlocal declared at outer level (g), and referenced by inner level (h)
def f():
    x = 1
    def g():
        nonlocal x
        def h():
            return x
        return h
    return g
print(f()()())
