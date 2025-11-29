"""
categories: Modules,typing
description: ``reveal_type()`` is not implemented.
cause: Micropython does not implement all typing features
workaround: None
"""

from typing import Self, reveal_type


class Foo:
    def return_self(self) -> Self:
        ...


class SubclassOfFoo(Foo):
    pass


foo = Foo()
sub = SubclassOfFoo()

reveal_type(foo)
reveal_type(sub)
