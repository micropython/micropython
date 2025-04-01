"""
categories: Syntax,Operators
description: MicroPython allows := to assign to the iteration variable in nested comprehensions, CPython does not.
cause: MicroPython is optimised for code size. Although it is a syntax error to assign to the iteration variable in a standard comprehension (same as CPython), it doesn't check if an inner nested comprehension assigns to the iteration variable of the outer comprehension.
workaround: Do not use := to assign to the iteration variable of a comprehension.
"""

print([[(j := i) for i in range(2)] for j in range(2)])
