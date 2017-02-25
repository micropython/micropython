"""
categories: Types,list
description: List store with step != 1 not implemented
cause: Unknown
workaround: Unknown
"""
l = [1, 2, 3, 4]
l[0:4:2] = [5, 6]
print(l)
