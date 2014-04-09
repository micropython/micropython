def f():
    x = 1
    y = 2
    def g():
        nonlocal x
        print(y)
        try:
            print(x)
        except NameError:
            print("NameError")
    def h():
        nonlocal x
        print(y)
        try:
            del x
        except NameError:
            print("NameError")
    print(x, y)
    del x
    g()
    h()
f()
