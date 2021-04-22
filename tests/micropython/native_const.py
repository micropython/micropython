# test loading constants in native functions


@micropython.native
def f():
    return b"bytes"


print(f())


@micropython.native
def f():
    @micropython.native
    def g():
        return 123

    return g


print(f()())
