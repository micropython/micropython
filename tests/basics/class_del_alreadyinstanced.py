try:
    import sys
    import gc
    if hasattr(sys.implementation, '_mpy'):
        import vfs # correlate with MICROPY_ENABLE_FINALISER
except ImportError:
    print("SKIP")
    raise SystemExit


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
