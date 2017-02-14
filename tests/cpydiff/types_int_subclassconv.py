"""
categories: Types,int
description: No int conversion for int-derived types available
cause: Unknown
workaround: Unknown
"""
class A(int):
    __add__ = lambda self, other: A(int(self) + other)

a = A(42)
print(a+a)
