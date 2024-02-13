"""
categories: Types,bytes
description: Bytes subscription with step != 1 not implemented
cause: MicroPython is highly optimized for memory usage.
workaround: Use explicit loop for this very rare operation.
"""
print(b"123"[0:3:2])
