"""
categories: Types,bytes
description: bytes() constructor only supports encoding arguments 'utf8', 'utf-8' and 'ascii'. Other encodings like 'latin-1' are not supported. Other string forms such as 'UTF8' are not supported.
cause: MicroPython is optimised for embedded systems and has limited codec support to save code size. `A similar restriction applies to str.encode() <cpydiff_types_str_encode_encoding>`. See also `unicode_support`.
workaround: Implement other encoding conversions manually by parsing the result of str.encode() or bytes() constructor.
"""

# Both CPython and MicroPython can encode this emoji as UTF-8 bytes
print(bytes("😀", "utf8"))

# Both CPython and MicroPython will fail to encode this emoji as ASCII bytes
try:
    print(bytes("😀", "ascii"))
except UnicodeError:
    print("UnicodeError")

# Other encodings or string formats aren't accepted by MicroPython
try:
    print(bytes("😀", "UTF-8"))
except LookupError:
    print("LookupError")
