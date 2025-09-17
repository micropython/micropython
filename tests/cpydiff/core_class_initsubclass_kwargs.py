"""
categories: Core,Classes
description: MicroPython doesn't support parameterized ``__init_subclass__`` class customization.
cause: MicroPython does not currently implement PEP 487. The MicroPython syntax tree does not include a kwargs node after the class inheritance list.
workaround: Use class variables or another mechanism to specify base-class customizations.
"""


class Base:
    @classmethod
    def __init_subclass__(cls, arg=None, **kwargs):
        cls.init_subclass_was_called = True
        print(f"Base.__init_subclass__({cls.__name__}, {arg=!r}, {kwargs=!r})")


class A(Base, arg="arg"):
    pass


# Regularize across MicroPython not automatically calling __init_subclass__ either.
if not getattr(A, "init_subclass_was_called", False):
    A.__init_subclass__()
