"""
categories: Types,list
description: List delete with step != 1 not implemented
cause: Unknown
workaround: Unknown
"""
l = [1, 2, 3, 4]
del l[0:4:2]
print(l)
