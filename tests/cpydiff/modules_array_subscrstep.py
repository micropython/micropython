"""
categories: Modules,array
description: Subscript with step != 1 is not yet implemented
cause: Unknown
workaround: Unknown
"""
import array

a = array.array("b", (1, 2, 3))
print(a[3:2:2])
