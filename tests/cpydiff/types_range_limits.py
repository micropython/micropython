"""
categories: Types,range
description: Range arguments must fit in a machine word; large start or stop values misbehave.
cause: Range stores its arguments as the C mp_int_t type, and intermediate calculations also use it.
workaround: Avoid using such ranges
"""

from sys import maxsize

# A range including `maxsize-1` cannot be created
try:
    print(range(-maxsize - 1, 0))
except OverflowError:
    print("OverflowError")

# A range with start or stop outside [-maxsize, maxsize] cannot be created, even if the range itself would be small.
try:
    print(range(maxsize + 1, maxsize + 2))
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
