# Tests various constants of the math module.
try:
    try:
        import umath as math
        from umath import exp, cos
    except ImportError:
        import math
        from math import exp, cos
except ImportError:
    print("SKIP")
    raise SystemExit

print(math.e == exp(1.0))

print(cos(math.pi))
