@micropython.viper
def fun():
    pass


try:
    print(fun.__name__)
except AttributeError:
    print("SKIP")
    raise SystemExit


class A:
    @micropython.viper
    def __init__(self):
        pass

    @micropython.viper
    def meth(self):
        pass


print(A.__init__.__name__)
print(A.meth.__name__)
print(A().meth.__name__)
