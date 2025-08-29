
import gc


class Foo:
    pass


def del_function(self):
    print("__del__")


f = Foo()
try:
    Foo.__del__ = del_function
except AttributeError:
    print("AttributeError")
del f

gc.collect()
