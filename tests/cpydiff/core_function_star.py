"""
categories: Core,Functions
description: ``*args`` cannot follow a keyword argument
cause: MicroPython is optimised for code space. For more information see `this issue <https://github.com/micropython/micropython/issues/11439>`_.
workaround: Re-order the arguments
"""


def f(x, y):
    return x + y


try:
    print(f(y=1, *(3,)))
except Exception as e:
    print(e)
