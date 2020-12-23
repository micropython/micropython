"""
categories: Modules,random
description: ``getrandbits`` method can only return a maximum of 32 bits at a time.
cause: PRNG's internal state is only 32bits so it can only return a maximum of 32 bits of data at a time.
workaround: If you need a number that has more than 32bits then utilize a function like getmorebits from the examples folder.
"""

import random


x = random.getrandbits(64)
print("{}".format(x))
