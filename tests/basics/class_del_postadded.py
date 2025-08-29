
import gc


class Foo:
    pass


def del_function(self):
    print("__del__")


Foo.__del__ = del_function
f = Foo()
del f

gc.collect()
