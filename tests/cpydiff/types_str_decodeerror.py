"""
categories: Types,str
description: UnicodeDecodeError not raised when expected
cause: Unknown
workaround: Unknown
"""
try:
    print(repr(str(b"\xa1\x80", 'utf8')))
    print('Should not get here')
except UnicodeDecodeError:
    print('UnicodeDecodeError')
