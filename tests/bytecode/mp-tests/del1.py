del x
del x.y
del x().y
del g
del x[a]
def f():
    global g
    del x
    del g
    local = 1
    local2 = 2
    local3 = 3
    del local, local2, local3
    def f2():
        nonlocal local3
        del local2, local3
