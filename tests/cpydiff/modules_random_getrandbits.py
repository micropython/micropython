"""
categories: Modules,random
description: ``getrandbits`` method can only return a maximum of 32 bits at a time.
cause: unknown
workaround: If you need a number that's larger than 32bits and want to use getrandbits use the code below.
"""

import random


x = random.getrandbits(64)
print("{} is {} bits".format(x))
