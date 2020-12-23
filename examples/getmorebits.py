from random import getrandbits


def getmorebits(bits: int) -> int:
    """
    :param bits: The number of bits to generate.
    :return: The resulting integer.
    """

    n = bits // 32
    d = 0
    for i in range(n):
        d |= getrandbits(32) << (i * 32)

    r = bits % 32
    if r >= 1:
        d |= getrandbits(r) << (n * 32)

    return d
