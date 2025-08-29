try:
    import sys
    import gc
    if hasattr(sys.implementation, '_mpy'):
        import vfs # correlate with MICROPY_ENABLE_FINALISER
except ImportError:
    print("SKIP")
    raise SystemExit


class Foo:
    def __del__(self):
        print("__del__")


f = Foo()

del f

gc.collect()
