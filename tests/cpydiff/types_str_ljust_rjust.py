"""
categories: Types,str
description: str.ljust() and str.rjust() not implemented
cause: MicroPython is highly optimized for memory usage. Easy workarounds available.
workaround: Instead of ``s.ljust(10)`` use ``"%-10s" % s``, instead of ``s.rjust(10)`` use ``"% 10s" % s``. Alternatively, ``"{:<10}".format(s)`` or ``"{:>10}".format(s)``.
"""

print("abc".ljust(10))
