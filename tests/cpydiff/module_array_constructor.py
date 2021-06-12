"""
categories: Modules,array
description: overflow checking is not implemented
cause: micropython implements implicit truncation in order to reduce code size and execution time
workaround: mask value
"""
import array

value = 257
a = array.array("b", [value & 0xff])
print(a)