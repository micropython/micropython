# test loading constants in viper functions

@micropython.viper
def f():
    return b'bytes'
print(f())

@micropython.viper
def f():
    @micropython.viper
    def g() -> int:
        return 123
    return g
print(f()())
