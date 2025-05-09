"""
categories: Syntax,Spaces
description: MicroPython requires spaces between literal numbers and keywords, CPython doesn't
cause: Different parser implementation

MicroPython's tokenizer treats a sequence like ``1and`` as a single token, while CPython treats it as two tokens.

Since CPython 3.11, this syntax causes a ``SyntaxWarning`` for an "invalid literal".

workaround: Add a space between the integer literal and the intended next token.

This also fixes the ``SyntaxWarning`` in CPython.
"""

try:
    print(eval("1and 0"))
except SyntaxError:
    print("Should have worked")
try:
    print(eval("1or 0"))
except SyntaxError:
    print("Should have worked")
try:
    print(eval("1if 1else 0"))
except SyntaxError:
    print("Should have worked")
