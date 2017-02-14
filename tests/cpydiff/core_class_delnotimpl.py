"""
categories: Core,Classes
description: Special method __del__ not implemented for user-defined classes
cause: Unknown
workaround: Unknown
"""
import gc

class Foo():
    def __del__(self):
        print('__del__')

f = Foo()
del f

gc.collect()
