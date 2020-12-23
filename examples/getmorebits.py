import random


def getmorebits(bits: int) -> int:
    n = bits // 32
    k = 0
    d = 0
    while k < n:
        d |= random.getrandbits(32) << (k * 32)
        k += 1
    d |= random.getrandbits(bits % 32) << (k * 32)

    return d
