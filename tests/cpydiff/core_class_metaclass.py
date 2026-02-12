"""
categories: Core,Classes
description: Metaclass __prepare__ method not supported by default
cause: MICROPY_METACLASS_PREPARE is disabled by default to save code size
workaround: Enable MICROPY_METACLASS_PREPARE if __prepare__ support is needed
"""


class Meta(type):
    @staticmethod
    def __prepare__(name, bases):
        print("__prepare__ called")
        return {}


class C(metaclass=Meta):
    pass
