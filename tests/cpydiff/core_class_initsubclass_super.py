"""
categories: Core,Classes
description: ``__init_subclass__`` can't be defined a cooperatively-recursive way.
cause: MicroPython does not currently implement PEP 487. The base object type does not have an ``__init_subclass__`` implementation.
workaround: Omit the recursive ``__init_subclass__`` call unless it's known that the grandparent also defines it.
"""


class Base:
    @classmethod
    def __init_subclass__(cls, **kwargs):
        cls.init_subclass_was_called = True
        super().__init_subclass__(**kwargs)


class A(Base):
    pass


# Regularize across MicroPython not automatically calling __init_subclass__ either.
if not getattr(A, "init_subclass_was_called", False):
    A.__init_subclass__()
