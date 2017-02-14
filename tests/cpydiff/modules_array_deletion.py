"""
categories: Modules,array
description: Array deletion not implemented
cause: Unknown
workaround: Unknown
"""
import array
a = array.array('b', (1, 2, 3))
del a[1]
print(a)
