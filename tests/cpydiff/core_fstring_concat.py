"""
categories: Core
description: f-strings don't support concatenation with adjacent literals if the adjacent literals contain braces
cause: MicroPython is optimised for code space.
workaround: Use the + operator between literal strings when they are not both f-strings
"""

x, y = 1, 2
# fmt: off
print("aa" f"{x}")  # works
print(f"{x}" "ab")  # works
print("a{}a" f"{x}")  # fails
print(f"{x}" "a{}b")  # fails
# fmt: on
