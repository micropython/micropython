"""
categories: Core,Classes
description: ``__init_subclass__`` isn't automatically called.
cause: MicroPython does not currently implement PEP 487.
workaround: Manually call ``__init_subclass__`` after class creation if needed. e.g.::

    class A(Base):
        pass
    A.__init_subclass__()

"""


class Base:
    @classmethod
    def __init_subclass__(cls):
        print(f"Base.__init_subclass__({cls.__name__})")


class A(Base):
    pass
