"""
categories: Syntax,Literals
description: MicroPython requires spaces between literal numbers and keywords or ".", CPython doesn't
cause: Different parser implementation

MicroPython's tokenizer treats a sequence like ``1and`` as a single token, while CPython treats it as two tokens.

Since CPython 3.11, when the literal number is followed by a token, this syntax causes a ``SyntaxWarning`` for an "invalid literal". When a literal number is followed by a "." denoting attribute access, CPython does not warn.

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
try:
    print(eval("0x1.to_bytes(1)"))
except SyntaxError:
    print("Should have worked")
