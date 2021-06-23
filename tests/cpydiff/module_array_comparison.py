"""
categories: Modules,array
description: Comparison between different typecodes not supported
cause: Code size
workaround: Compare individual elements
"""
import array

array.array("b", [1, 2]) == array.array("i", [1, 2])
