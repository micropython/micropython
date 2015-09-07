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
