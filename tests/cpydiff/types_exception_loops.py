"""
categories: Types,Exception
description: Exception in while loop condition may have unexpected line number
cause: Condition checks are optimized to happen at the end of loop body, and that line number is reported.
workaround: Unknown
"""

l = ["-foo", "-bar"]

i = 0
while l[i][0] == "-":
    print("iter")
    i += 1
