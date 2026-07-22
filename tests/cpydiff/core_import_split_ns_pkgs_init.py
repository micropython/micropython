"""
categories: Core,import
description: MicroPython historically did not support namespace packages split across filesystem.
cause: MicroPython's import system is highly optimized for simplicity, minimal memory usage, and minimal filesystem search overhead.
workaround: Not required, fixed.
"""

import sys

sys.path.append(sys.path[1] + "/modules3")
sys.path.append(sys.path[1] + "/modules2")

# import from the first subpackage first
import subpkg.baz
import subpkg.bar

print("Two modules of a split non-namespace package imported")

import subpkg

assert subpkg.two == 2

print("__init__ is used from the second module")
print(subpkg.__path__)
