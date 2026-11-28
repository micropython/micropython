"""
categories: Modules,collections,namedtuple
description: Reserved and repeated field names are accepted by the namedtuple constructor
cause: MicroPython is optimised for code space.
workaround: Do not use reserved words as namedtuple field names and do not repeat field names
"""

import collections

# def is a reserved keyword
try:
    T = collections.namedtuple("T", ("def",))
    print(T)
except ValueError:
    print("ValueError")

# The field name "abc" is repeated
try:
    T = collections.namedtuple("T", ("abc", "xyz", "abc"))
    print(T)
except ValueError:
    print("ValueError")

# Field names beginning with an underscore are reserved
try:
    T = collections.namedtuple("T", ("_a",))
    print(T)
except ValueError:
    print("ValueError")
