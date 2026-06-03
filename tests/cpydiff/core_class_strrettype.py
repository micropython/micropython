"""
categories: Core,Classes
description: ``__str__`` returning non-string type does not raise TypeError
cause: MicroPython's instance_print does not validate that ``__str__`` or ``__repr__`` return a str or its subclass
workaround: Ensure ``__str__`` and ``__repr__`` always return a str instance or its subclass
"""


class Foo:
    def __str__(self):
        return True


print(str(Foo()))
