"""
categories: Syntax,Spaces
description: uPy requires spaces between literal numbers and keywords, CPy doesn't
cause: Unknown
workaround: Unknown
"""
try:
    print(eval('1and 0'))
except SyntaxError:
    print('Should have worked')
try:
    print(eval('1or 0'))
except SyntaxError:
    print('Should have worked')
try:
    print(eval('1if 1else 0'))
except SyntaxError:
    print('Should have worked')
