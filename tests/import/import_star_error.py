# test errors with import *

if not hasattr(object, "__init__"):
    # target doesn't have MICROPY_CPYTHON_COMPAT enabled, so doesn't check for "import *"
    print("SKIP")
    raise SystemExit

# 'import *' is not allowed in function scope
try:
    exec("def foo(): from x import *")
except SyntaxError as er:
    print("function", "SyntaxError")

# 'import *' is not allowed in class scope
try:
    exec("class C: from x import *")
except SyntaxError as er:
    print("class", "SyntaxError")
