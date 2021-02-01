"""
categories: Core,import
description: __all__ is unsupported in __init__.py in MicroPython.
cause: Not implemented.
workaround: Manually import the sub-modules directly in __init__.py using ``from . import foo, bar``.
"""
from modules3 import *

foo.hello()
