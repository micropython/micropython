"""
categories: Core,Classes
description: dir() does not convert __dir__ return value to a sorted list
cause: MicroPython's dir() returns the value from __dir__ as-is, without iterating it into a list or sorting it.
workaround: Have __dir__ return a sorted list directly.
"""


class C:
    def __dir__(self):
        return "cba"


print(dir(C()))
