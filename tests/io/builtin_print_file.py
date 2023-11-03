# test builtin print function, using file= argument

import sys

try:
    sys.stdout
except AttributeError:
    print("SKIP")
    raise SystemExit

print(file=sys.stdout)
print("test", file=sys.stdout)

try:
    print(file=1)
except (AttributeError, OSError):  # CPython and uPy differ in error message
    print("Error")
