"""
categories: Types,bytes
description: bytes.decode() does not accept keyword arguments, only positional arguments
cause: MicroPython optimizes for code size and does not implement keyword argument handling for bytes.decode()
workaround: Use positional arguments instead of keyword arguments
"""

# CPython accepts keyword arguments, MicroPython only accepts positional
b = b"hello\xffworld"

try:
    # Using keyword arguments
    result = b.decode(encoding="utf-8", errors="ignore")
    print("kwargs supported:", repr(result))
except TypeError as e:
    print("kwargs not supported: TypeError")
    # Workaround: use positional arguments
    result = b.decode("utf-8", "ignore")
    print("positional args work:", repr(result))
