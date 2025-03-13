"""
categories: Core,Classes
description: ``__set_name__`` is not automatically called on applicable class members.
cause: MicroPython does not currently implement PEP 487.
workaround: Manually iterate through a class's ``__dict__`` and invoke ``__set_name__`` on its members after creating a class.
"""


class Descriptor:
    def __set_name__(self, owner, name):
        print(f"Descriptor.__set_name__({owner.__name__}, {name!r})")


class A:
    x = Descriptor()
