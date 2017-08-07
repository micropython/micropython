"""
categories: Core,Classes
description: Method Resolution Order (MRO) is not compliant with CPython
cause: Depth first non-exhaustive method resolution order
workaround: Avoid complex class hierarchies with multiple inheritance and complex method overrides. Keep in mind that many languages don't support multiple inheritance at all.
"""
class Foo:
    def __str__(self):
        return "Foo"

class C(tuple, Foo):
    pass

t = C((1, 2, 3))
print(t)
