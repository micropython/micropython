"""
categories: Types,Exception
description: All exceptions have readable ``value`` and ``errno`` attributes, not just ``StopIteration`` and ``OSError``.
cause: MicroPython is optimised to reduce code size.
workaround: Only use ``value`` on ``StopIteration`` exceptions, and ``errno`` on ``OSError`` exceptions.  Do not use or rely on these attributes on other exceptions.
"""

e = Exception(1)
print(e.value)
print(e.errno)
