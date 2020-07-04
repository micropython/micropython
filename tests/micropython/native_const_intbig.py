# check loading constants


@micropython.native
def f():
    return 123456789012345678901234567890


print(f())
