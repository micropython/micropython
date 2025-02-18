"""
categories: Modules,builtins
description: Second argument to next() is not implemented
cause: MicroPython is optimised for code space.
workaround: Instead of ``val = next(it, deflt)`` use::

    try:
        val = next(it)
    except StopIteration:
        val = deflt
"""

print(next(iter(range(0)), 42))
