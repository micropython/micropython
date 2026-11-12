"""
categories: Core,import
description: MicroPython historically did not support namespace packages split across filesystem.
cause: MicroPython's import system is highly optimized for simplicity, minimal memory usage, and minimal filesystem search overhead.
workaround: Not required, fixed.
"""

import sys

sys.path.append(sys.path[1] + "/modules")
sys.path.append(sys.path[1] + "/modules2")

# import from the second subpackage first
import subpkg.bar
import subpkg.foo

print("Two modules of a split non-namespace package imported")

import subpkg

assert subpkg.one == 1
assert not hasattr(subpkg, "two")

print("The first module's __init__ is used")
