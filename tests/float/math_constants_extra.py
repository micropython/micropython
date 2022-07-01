# Tests constants of the math module available only with MICROPY_PY_MATH_CONSTANTS.

try:
    try:
        import umath as math
        from umath import isnan
    except ImportError:
        import math
        from math import isnan

    math.tau
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

print(math.tau == 2.0 * math.pi)

print(math.inf == float("inf"))
print(-math.inf == -float("inf"))

print(isnan(math.nan))
print(isnan(-math.nan))
