# test with handling within a native function


class C:
    def __init__(self):
        print("__init__")

    def __enter__(self):
        print("__enter__")

    def __exit__(self, a, b, c):
        print("__exit__", a, b, c)


# basic with
@micropython.native
def f():
    with C():
        print(1)


f()

# nested with and try-except
@micropython.native
def f():
    try:
        with C():
            print(1)
            fail
            print(2)
    except NameError:
        print("NameError")


f()
