# test from ... import * when module __dict__ is not a dict

import sys

if not hasattr(object, "__init__"):
    # target doesn't have MICROPY_CPYTHON_COMPAT enabled
    print("SKIP")
    raise SystemExit


class M:
    pass


m = M()
try:
    m.__dict__ = 42
    sys.modules["m"] = m
    from m import *
except TypeError:
    print("TypeError")
