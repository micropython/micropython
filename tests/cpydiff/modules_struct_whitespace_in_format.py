"""
categories: Modules,struct
description: Struct pack with whitespace in format,
witespace ignored by CPython, error on uPy
cause: Unknown
workaround: Unknown
"""
import struct

try:
    print(struct.pack('b b', 1, 2))
    print('Should have worked')
except:
    print('struct.error')
