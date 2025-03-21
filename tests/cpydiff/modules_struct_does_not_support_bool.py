"""
categories: Modules,struct
description: Struct ``pack`` and ``unpack`` does not support the "?" type code for bools.
cause: Unknown
workaround: Cast to int and pack with "B" type code, then unpack with "B" and cast to bool.
"""

import struct

struct.pack("?", True)
struct.unpack("?", b"\x01")
