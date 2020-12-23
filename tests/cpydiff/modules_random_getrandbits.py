"""
categories: Modules,random
description: ``getrandbits`` method can only return a maximum of 32 bits at a time.
cause: Internal PRNG's state is only 32 bits.
workaround: If you need a number that's larger than 32bits and want to use getrandbits use a function like the one in the examples folder called "getmorebits.py". The function contained within will let you work around this issue if you need more than 32 bits of random data.
"""

import random

x = random.getrandbits(65)
print("{} is bits".format(x))
