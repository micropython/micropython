"""
categories: Types,complex
description: MicroPython's complex() accepts certain incorrect values that CPython rejects
cause: MicroPython is highly optimized for memory usage.
workaround: Do not use non-standard complex literals as argument to complex()

MicroPython's ``complex()`` function accepts literals that contain a space and
no sign between the real and imaginary parts, and interprets it as a plus.
"""

try:
    print(complex("1 1j"))
except ValueError:
    print("ValueError")
