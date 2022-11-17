"""
categories: Core,Classes
description: When inheriting native types, calling a method in ``__init__(self, ...)`` before ``super().__init__()`` raises an ``AttributeError`` (or segfaults if ``MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG`` is not enabled).
cause: MicroPython does not have separate ``__new__`` and ``__init__`` methods in native types.
workaround: Call ``super().__init__()`` first.
"""


class L1(list):
    def __init__(self, a):
        self.append(a)


try:
    L1(1)
    print("OK")
except AttributeError:
    print("AttributeError")


class L2(list):
    def __init__(self, a):
        super().__init__()
        self.append(a)


try:
    L2(1)
    print("OK")
except AttributeError:
    print("AttributeError")
