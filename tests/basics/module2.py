# uPy behaviour only: builtin modules are read-only
import sys
try:
    sys.x = 1
except AttributeError:
    print("AttributeError")
