"""
categories: Types,Exception
description: While loop guards will obscure exception line number reporting due to being optimised onto the end of the code block
cause: Unknown
workaround: Unknown
"""
l = ["-foo", "-bar"]

i = 0
while l[i][0] == "-":
    print("iter")
    i += 1
