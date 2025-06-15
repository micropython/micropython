# Test expressions based on math module constants
try:
    import math
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

from micropython import const

# check that we can make consts from math constants
two_pi = const(2.0 * math.pi)
print(math.cos(two_pi))
