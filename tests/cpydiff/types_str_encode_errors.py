"""
categories: Types,str
description: str.encode() and bytes() constructor ignore any ``errors`` argument specified. If the encoding is specified as ``'ascii'`` and a non-ASCII byte is found in the string then an exception is always raised.
cause: MicroPython is optimised for embedded systems and has limited codec support to save code size. See also `unicode_support`.
workaround: Handle encoding errors manually by parsing the result of str.encode() or bytes() constructor.
"""

# CPython will replace the emoji with an ASCII '?' but MicroPython will
# raise an exception
try:
    print("😀".encode("ascii", "replace"))
except UnicodeError:
    print("UnicodeError")

# CPython will ignore the emoji in the result but MicroPython will raise an exception
try:
    print("😀".encode("ascii", "ignore"))
except UnicodeError:
    print("UnicodeError")
