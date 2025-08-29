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


Foo.__del__ = del_function
f = Foo()

del f

gc.collect()
