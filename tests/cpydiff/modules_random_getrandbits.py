"""
categories: Modules,random
description: ``getrandbits`` method can only return a maximum of 32 bits at a time.
cause: Internal PRNG's state is only 32 bits.
workaround: If you need a number that's larger than 32bits and want to use getrandbits use the code below.
"""

import random

x = random.getrandbits(64)
print("{} is bits".format(x))


def getmorebits(bits: int) -> int:
    """
    :param bits: The number of bits to generate.
    :return: The resulting integer.
    """

    n = bits // 32
    d = 0
    for i in range(n):
        d |= random.getrandbits(32) << (i * 32)
    d |= random.getrandbits(bits % 32) << (n * 32)

    return d
