"""
categories: Core,Functions
description: Function objects do not have the ``__module__`` attribute
cause: MicroPython is optimized for reduced code size and RAM usage.
workaround: Use ``sys.modules[function.__globals__['__name__']]`` for non-builtin modules.
"""


def f():
    pass


print(f.__module__)
