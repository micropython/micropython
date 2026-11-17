"""
categories: Types,str
description: str(...) with keywords not implemented
cause: Unknown
workaround: Input the encoding format directly. eg ``print(bytes('abc', 'utf-8'))``
"""

print(str(b"abc", encoding="utf8"))
