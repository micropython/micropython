def __getattr__(attr):
    # A real bug in __getattr__ (not "attribute not found") must propagate
    # out of `from pkg import *`, not be swallowed as if __all__ were
    # simply absent.
    raise ValueError("bug, not attribute-not-found")
