"""
categories: Types,Exception
description: Exception.__init__ raises TypeError if overridden and called by subclass
cause: Unknown
workaround: Unknown
"""
class A(Exception):
    def __init__(self):
        Exception.__init__(self)

a = A()
