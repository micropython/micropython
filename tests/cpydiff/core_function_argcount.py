"""
categories: Core,Functions
description: Error messages for methods may display unexpected argument counts
cause: MicroPython counts "self" as an argument.
workaround: Interpret error messages with the information above in mind.
"""
try:
    [].append()
except Exception as e:
    print(e)
