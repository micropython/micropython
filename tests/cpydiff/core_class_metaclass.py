"""
categories: Core,Classes
description: Defining a class with a metaclass is not possible.
cause: Metaclasses are not supported in MicroPython.
workaround: None
"""

from abc import ABCMeta


class MyABC(metaclass=ABCMeta):
    pass
