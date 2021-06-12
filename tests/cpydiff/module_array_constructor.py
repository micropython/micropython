"""
categories: Modules,array
description: overflow checking is not implemented
cause: micropython implements implicit truncation in order to reduce code size and execution time
workaround: if CPython compatibility is needed then mask the value explicitly
"""
import array

a = array.array("b", [257])
print(a)