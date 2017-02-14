"""
categories: Types,str
description: Instance of a subclass of str cannot be compared for equality with an instance of a str
cause: Unknown
workaround: Unknown
"""
class S(str):
    pass

s = S('hello')
print(s == 'hello')
