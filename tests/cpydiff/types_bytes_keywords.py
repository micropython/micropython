"""
categories: Types,bytes
description: bytes() with keywords not implemented
cause: Unknown
workaround: Pass the encoding as a positional parameter, e.g. ``print(bytes('abc', 'utf-8'))``
"""
print(bytes("abc", encoding="utf8"))
