# Test expressions based on math module constants
try:
    import math
except ImportError:
    print("SKIP")
    raise SystemExit

from micropython import const

# check that we can make consts from math constants
# (skip if the target has MICROPY_COMP_MODULE_CONST disabled)
try:
    exec("two_pi = const(2.0 * math.pi)")
except SyntaxError:
    print("SKIP")
    raise SystemExit

print(math.cos(two_pi))
