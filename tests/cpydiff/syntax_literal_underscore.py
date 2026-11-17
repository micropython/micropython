"""
categories: Syntax,Literals
description: MicroPython accepts underscores in numeric literals where CPython doesn't
cause: Different parser implementation

MicroPython's tokenizer ignores underscores in numeric literals, while CPython
rejects multiple consecutive underscores and underscores after the last digit.

workaround: Remove the underscores not accepted by CPython.
"""

try:
    print(eval("1__1"))
except SyntaxError:
    print("Should not work")
try:
    print(eval("1_"))
except SyntaxError:
    print("Should not work")
