"""
categories: Types,range
description: Range objects with large start or stop arguments misbehave.
cause: Intermediate calculations overflow the C mp_int_t type
workaround: Avoid using such ranges
"""

from sys import maxsize

# A range including `maxsize-1` cannot be created
try:
    print(range(-maxsize - 1, 0))
except OverflowError:
    print("OverflowError")

# A range with `stop-start` exceeding sys.maxsize has incorrect len(), while CPython cannot calculate len().
try:
    print(len(range(-maxsize, maxsize)))
except OverflowError:
    print("OverflowError")

# A range with `stop-start` exceeding sys.maxsize has incorrect len()
try:
    print(len(range(-maxsize, maxsize, maxsize)))
except OverflowError:
    print("OverflowError")
