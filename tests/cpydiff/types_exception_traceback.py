"""
categories: Types,Exception
description: The ``__traceback__`` attribute of exceptions is a simple list of tuples.
cause: MicroPython is optimised to reduce code size.
workaround: Check the type of ``__traceback__`` before using it.
"""


def foo():
    1 / 0


try:
    foo()
except Exception as e:
    print(e.__traceback__)
