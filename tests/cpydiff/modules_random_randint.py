"""
categories: Modules,random
description: ``randint`` method can only return an integer that is at most the native word size.
cause: The return type is cast to a native int thus causing and overflow.
workaround: If you need integers larger than native wordsize call randint multiple times and shift result.
"""

import random

x = random.randint(2 ** 128 - 1, 2 ** 128)
print("x={}".format(x))
