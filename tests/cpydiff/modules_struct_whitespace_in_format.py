"""
categories: Modules,struct
description: Struct pack with whitespace in format, whitespace ignored by CPython, error on uPy
cause: MicroPython is optimised for code size.
workaround: Don't use spaces in format strings.
"""
import struct

try:
    print(struct.pack("b b", 1, 2))
    print("Should have worked")
except:
    print("struct.error")
