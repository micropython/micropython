@d
def f():
    pass

@d
@e
def g():
    pass

@d.e.f
def h():
    pass

@d(a + 1)
def i():
    pass

@d(a + 1, b + 2)
def i():
    pass
