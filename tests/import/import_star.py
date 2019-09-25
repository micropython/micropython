
try:
    exec('def foo(): from import1b import *')
except SyntaxError as er:
    # MICROPY_CPYTHON_COMPAT: 'import *' is not allowed in function scope
    print(123)
else:
    # 'import *' always updates global scope
    foo()
    print(var)

try:
    exec('class C: from import1b import *')
except SyntaxError as er:
    # MICROPY_CPYTHON_COMPAT: 'import *' is not allowed in class scope
    print(123)
else:
    # 'import *' always updates global scope
    print(var)
