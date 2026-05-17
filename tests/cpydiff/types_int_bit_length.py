"""
categories: Types,int
description: ``bit_length`` method doesn't exist.
cause: bit_length method is not implemented.
workaround: Avoid using this method on MicroPython.
"""

x = 255
print("{} is {} bits long.".format(x, x.bit_length()))
