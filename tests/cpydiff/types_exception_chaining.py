"""
categories: Types,Exception
description: Exception chaining not implemented
cause: Unknown
workaround: Unknown
"""
try:
    raise TypeError
except TypeError:
    raise ValueError
