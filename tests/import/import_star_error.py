# test errors with import *

# 'import *' is not allowed in function scope
try:
    exec('def foo(): from x import *')
except SyntaxError as er:
    print('function', 'SyntaxError')

# 'import *' is not allowed in class scope
try:
    exec('class C: from x import *')
except SyntaxError as er:
    print('class', 'SyntaxError')
