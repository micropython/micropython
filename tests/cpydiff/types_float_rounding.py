"""
categories: Types,float
description: uPy and CPython outputs formats differ
cause: Unknown
workaround: Unknown
"""
print('%.1g' % -9.9)
print('%.1e' % 9.99)
print('%.1e' % 0.999)
