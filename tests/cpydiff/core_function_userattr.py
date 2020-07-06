"""
categories: Core,Functions
description: User-defined attributes for functions are not supported
cause: MicroPython is highly optimized for memory usage.
workaround: Use external dictionary, e.g. ``FUNC_X[f] = 0``.
"""


def f():
    pass


f.x = 0
print(f.x)
