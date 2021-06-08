"""
categories: Modules,random
description: ``randint`` method can only return an integer that is at most the native word size.
cause: PRNG is only able to generate 32 bits of state at a time. The result is then cast into a native sized int instead of a full int object.
workaround: If you need integers larger than native wordsize use the random module from micropython-lib.
"""

import random


x = random.randint(2 ** 128 - 1, 2 ** 128)
print("x={}".format(x))
