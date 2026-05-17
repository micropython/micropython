"""
categories: Types,str
description: MicroPython accepts but does not properly implement the "," or "_" grouping character for float values
cause: To reduce code size, MicroPython does not implement this combination. Grouping characters will not appear in the number's significant digits and will appear at incorrect locations in leading zeros.
workaround: Do not use a format string like ``{:,f}`` if exact CPython compatibility is required.
"""

print("{:,f}".format(3141.159))
print("{:_f}".format(3141.159))
print("{:011,.2f}".format(3141.159))
print("{:011_.2f}".format(3141.159))
