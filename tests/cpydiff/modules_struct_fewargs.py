"""
categories: Modules,struct
description: Struct pack with too few args, not checked by uPy
cause: Unknown
workaround: Unknown
"""
import struct

try:
    print(struct.pack("bb", 1))
    print("Should not get here")
except:
    print("struct.error")
