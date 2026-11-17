"""
categories: Types,list
description: List slice-store with non-iterable on RHS is not implemented
cause: RHS is restricted to be a tuple or list
workaround: Use ``list(<iter>)`` on RHS to convert the iterable to a list
"""

l = [10, 20]
l[0:1] = range(4)
print(l)
