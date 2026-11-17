"""
categories: Types,str
description: MicroPython accepts the "," grouping option with any radix, unlike CPython
cause: To reduce code size, MicroPython does not issue an error for this combination
workaround: Do not use a format string like ``{:,b}`` if CPython compatibility is required.
"""

try:
    print("{:,b}".format(99))
except ValueError:
    print("ValueError")
try:
    print("{:,x}".format(99))
except ValueError:
    print("ValueError")
try:
    print("{:,o}".format(99))
except ValueError:
    print("ValueError")
