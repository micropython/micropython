"""
categories: Core
description: f-strings cannot support expressions that require parsing to resolve nested braces or colons
cause: MicroPython is optimised for code space.
workaround: Only use simple expressions inside f-strings
"""

f'{"hello {} world"}'
f"{repr({})}"
f"{x[1:3]}"
