"""
categories: Types,Exception
description: User-defined attributes for builtin exceptions are not supported
cause: MicroPython is highly optimized for memory usage.
workaround: Use user-defined exception subclasses.
"""

e = Exception()
e.x = 0
print(e.x)
