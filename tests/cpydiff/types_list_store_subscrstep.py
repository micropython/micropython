"""
categories: Types,list
description: List store with step != 1 not implemented
cause: Unknown
workaround: Use explicit loop for this rare operation.
"""

l = [1, 2, 3, 4]
l[0:4:2] = [5, 6]
print(l)
