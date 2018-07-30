"""
categories: Types,Exception
description: Exception.__init__ method does not exist.
cause: Subclassing native classes is not fully supported in MicroPython.
workaround: Call using ``super()`` instead::

    class A(Exception):
        def __init__(self):
            super().__init__()
"""
class A(Exception):
    def __init__(self):
        Exception.__init__(self)

a = A()
