"""
categories: Syntax,Unpacking
description: Argument unpacking does not work if the argument being unpacked is the nth or greater argument where n is the number of bits in an MP_SMALL_INT.
cause: The implementation uses an MP_SMALL_INT to flag args that need to be unpacked.
workaround: Use fewer arguments.
"""


def example(*args):
    print(len(args))


MORE = ["a", "b", "c"]

# fmt: off
example(
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    *MORE,
)
# fmt: on
