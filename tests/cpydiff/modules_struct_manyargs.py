"""
categories: Modules,struct
description: Struct pack with too many args, not checked by MicroPython
cause: Unknown
workaround: Unknown
"""

import struct

try:
    print(struct.pack("bb", 1, 2, 3))
    print("Should not get here")
except:
    print("struct.error")
