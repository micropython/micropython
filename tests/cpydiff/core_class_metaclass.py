"""
categories: Core,Classes
description: Defining a class with a metaclass is not possible.
cause: Unknown
workaround: Unknown
"""

from abc import ABCMeta


class MyABC(metaclass=ABCMeta):
    pass
