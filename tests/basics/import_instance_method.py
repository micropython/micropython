# Test importing a method from a class instance.
# This is not a common thing to do, but ensures MicroPython has the same semantics as CPython.

import sys

if not hasattr(sys, "modules"):
    print("SKIP")
    raise SystemExit


class A:
    def __init__(self, value):
        self.value = value

    def meth(self):
        return self.value

    def meth_with_arg(self, a):
        return [self.value, a]


# Register a class instance as the module "mod".
sys.modules["mod"] = A(1)

# Try importing it as a module.
import mod

print(mod.meth())
print(mod.meth_with_arg(2))

# Change the module.
sys.modules["mod"] = A(3)

# Try importing it using "from ... import".
from mod import meth, meth_with_arg

print(meth())
print(meth_with_arg(4))
