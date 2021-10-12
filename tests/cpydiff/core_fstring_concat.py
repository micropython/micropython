"""
categories: Core
description: f-strings don't support concatenation with adjacent literals if the adjacent literals contain braces
cause: MicroPython is optimised for code space.
workaround: Use the + operator between literal strings when either is an f-string
"""

x = 1
print("aa" f"{x}")
print(f"{x}" "ab")
print("a{}a" f"{x}")
print(f"{x}" "a{}b")
