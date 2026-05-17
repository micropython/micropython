# Test "from x import *" where x is something other than a module.

import sys

try:
    next(iter([]), 42)
except TypeError:
    # Two-argument version of next() not supported.  We are probably not at
    # MICROPY_CONFIG_ROM_LEVEL_BASIC_FEATURES which is needed for "import *".
    print("SKIP")
    raise SystemExit

print("== test with a class as a module ==")


class M:
    x = "a1"

    def __init__(self):
        self.x = "a2"


sys.modules["mod"] = M
from mod import *

print(x)

sys.modules["mod"] = M()
from mod import *

print(x)

print("== test with a class as a module that overrides __all__ ==")


class M:
    __all__ = ("y",)
    x = "b1"
    y = "b2"

    def __init__(self):
        self.__all__ = ("x",)
        self.x = "b3"
        self.y = "b4"


sys.modules["mod"] = M
x = None
from mod import *

print(x, y)

sys.modules["mod"] = M()
from mod import *

print(x, y)

print("== test with objects that don't have a __dict__ ==")

sys.modules["mod"] = 1
try:
    from mod import *
    # MicroPython raises AttributeError, CPython raises ImportError.
except (AttributeError, ImportError):
    print("ImportError")
