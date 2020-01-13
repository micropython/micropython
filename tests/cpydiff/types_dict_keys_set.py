"""
categories: Types,dict
description: Dictionary keys view does not behave as a set.
cause: Not implemented.
workaround: Explicitly convert keys to a set before using set operations.
"""
print({1:2, 3:4}.keys() & {1})
