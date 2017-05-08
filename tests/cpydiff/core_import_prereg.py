"""
categories: Core,import
description: Failed to load modules are still registered as loaded
cause: To make module handling more efficient, it's not wrapped with exception handling.
workaround: Test modules before production use; during development, use ``del sys.modules["name"]``, or just soft or hard reset the board.
"""
import sys

try:
    from modules import foo
except NameError as e:
    print(e)
try:
    from modules import foo
    print('Should not get here')
except NameError as e:
    print(e)
