# check loading constants
# requires: int-big native


@micropython.native
def f():
    return 123456789012345678901234567890


print(f())
