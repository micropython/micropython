# uPy behaviour only: builtin modules are read-only
import usys
try:
    usys.x = 1
except AttributeError:
    print("AttributeError")
