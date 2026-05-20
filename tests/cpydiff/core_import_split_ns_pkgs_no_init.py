"""
categories: Core,import
description: MicroPython historically did not support namespace packages split across filesystem.
cause: MicroPython's import system is highly optimized for simplicity, minimal memory usage, and minimal filesystem search overhead.
workaround: Not required, fixed.
"""

import sys

sys.path.append(sys.path[1] + "/modules3")
sys.path.append(sys.path[1] + "/modules4")

# import from the first subpackage first
import subpkg.baz
import subpkg.quux

print("Two modules of a split namespace package imported")

import subpkg

assert not hasattr(subpkg, "two")

print("__path__ is from the first module")
print(subpkg.__path__)
