"""
categories: Types,int
description: Incorrect error message when passing float into to_bytes
cause: Unknown
workaround: Unknown
"""
try:
    int('1').to_bytes(1.0)
except TypeError as e:
    print(e)
