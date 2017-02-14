"""
categories: Core
description: Method Resolution Order (MRO) is not compliant with CPython
cause: Unknown
workaround: Unknown
"""
class Foo:
    def __str__(self):
        return "Foo"

class C(tuple, Foo):
    pass

t = C((1, 2, 3))
print(t)
