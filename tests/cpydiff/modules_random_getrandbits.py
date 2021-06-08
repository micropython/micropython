"""
categories: Modules,random
description: ``getrandbits`` method can only return a maximum of 32 bits at a time.
cause: PRNG's internal state is only 32bits so it can only return a maximum of 32 bits of data at a time.
workaround: If you need a number that has more than 32 bits then utilize the random module from micropython-lib.
"""

import random


x = random.getrandbits(64)
print("{}".format(x))
