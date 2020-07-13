"""
categories: Syntax,Operators
description: MicroPython allows using := to assign to the variable of a comprehension, CPython raises a SyntaxError.
cause: MicroPython is optimised for code size and doesn't check this case.
workaround: Do not rely on this behaviour if writing CPython compatible code.
"""
print([i := -1 for i in range(4)])
