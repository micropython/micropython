"""
categories: Core,Exceptions
description: Throwing a derived exception class instance in its `__init__` without first calling ``super().__init__`` is a TypeError
cause: In MicroPython, an object is incompletely constructed if it does not call its superclass init function or return normally from its ``__init__``. This prevents its usage in some circumstances.
workaround: Call the superclass `__init__` method before raising the exception.
"""


class C(Exception):
    def __init__(self):
        raise self


class C1(Exception):
    def __init__(self):
        super().__init__()
        raise self


try:
    C()
except Exception as e:
    print(type(e).__name__)

try:
    C1()
except Exception as e:
    print(type(e).__name__)
