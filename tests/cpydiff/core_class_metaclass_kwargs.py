"""
categories: Core,Classes
description: Additional keyword arguments in class definition not passed to metaclass
cause: Simplified implementation for code size optimization - only metaclass= keyword is extracted
workaround: Use metaclass __init__ to customize class creation instead of passing kwargs
"""


class Meta(type):
    def __init__(cls, name, bases, dct, **kwargs):
        print("kwargs:", kwargs)
        super().__init__(name, bases, dct)


class C(metaclass=Meta, foo="bar"):
    pass
