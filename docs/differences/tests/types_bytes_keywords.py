"""
categories: Types,bytes
description: bytes(...) with keywords not implemented
cause: Unknown
workaround: Input the encoding format directly. eg. ``print(bytes('abc', 'utf-8'))``
"""
print(bytes('abc', encoding='utf8'))
