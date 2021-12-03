"""
categories: Core
description: f-strings don't support the !r, !s, and !a conversions
cause: MicroPython is optimised for code space.
workaround: Use repr(), str(), and ascii() explictly.
"""


class X:
    def __repr__(self):
        return "repr"

    def __str__(self):
        return "str"


print(f"{X()!r}")
print(f"{X()!s}")
