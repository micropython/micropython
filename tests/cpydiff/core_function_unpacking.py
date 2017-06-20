"""
categories: Core,Functions
description: Unpacking function arguments in non-last position isn't detected as an error
cause: Unknown
workaround: The syntax below is invalid, never use it in applications.
"""
print(*(1, 2), 3)
