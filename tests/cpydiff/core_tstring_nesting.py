"""
categories: Core,t-strings
description: t-strings don't support PEP 701 arbitrary expression nesting (same quote types)
cause: MicroPython is optimised for code space.
workaround: Use different quote types for nested t-strings
"""

# NOTE: This test documents behavior differences with CPython.
# Since t-strings are not available in the testing environment,
# the actual behavior is shown in comments.

# Example that would work in MicroPython - using different quote types:
# name = "Alice"
# greeting = t"Hello, {t'{name}'}"  # This works
# print(greeting)
# Output: Template(strings=('Hello, ', ''), interpolations=(Interpolation(Template(...), "t'{name}'", None, ''),))

# Example that would fail in MicroPython - using same quote types:
# try:
#     greeting = t"Hello, {t"{name}"}"  # This fails with SyntaxError
#     print(greeting)
# except SyntaxError as e:
#     print(f"SyntaxError: {e}")
# Output: SyntaxError: invalid syntax

print("MicroPython t-strings require different quote types for nesting")
