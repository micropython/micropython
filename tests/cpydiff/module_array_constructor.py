"""
categories: Modules,array
description: Overflow checking is not implemented
cause: MicroPython implements implicit truncation in order to reduce code size and execution time
workaround: If CPython compatibility is needed then mask the value explicitly
"""
import array

a = array.array("b", [257])
print(a)
