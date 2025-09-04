"""
categories: Types,str
description: MicroPython cannot format non-ASCII characters with %c or {:c}
cause: To reduce code size, MicroPython does not implement this feature
workaround: Use %s or {:s} together with chr()
"""

# note: UTF-8 code point 233 is latin small letter e with acute
try:
    print("{:c}".format(233))
except Exception as e:
    print(type(e).__name__)

try:
    print("%c" % 233)
except Exception as e:
    print(type(e).__name__)

try:
    print("%c" % chr(233))
except Exception as e:
    print(type(e).__name__)

print()
print("Compatible alternatives:")

print("{:s}".format(chr(233)))
print("%s" % chr(233))
