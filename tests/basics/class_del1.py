"""
categories: Core,Classes
description: Special method __del__ is implemented for user-defined classes
cause: Known
workaround: Unnecessary
"""

import gc


class Foo:
    def __del__(self):
        print("__del__")


f = Foo()
del f

gc.collect()
