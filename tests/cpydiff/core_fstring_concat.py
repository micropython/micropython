"""
categories: Core
description: f-strings don't support concatenation with adjacent literals if the adjacent literals contain braces or are f-strings
cause: MicroPython is optimised for code space.
workaround: Use the + operator between literal strings when either or both are f-strings
"""

x, y = 1, 2
print("aa" f"{x}")  # works
print(f"{x}" "ab")  # works
print("a{}a" f"{x}")  # fails
print(f"{x}" "a{}b")  # fails
print(f"{x}" f"{y}")  # fails
