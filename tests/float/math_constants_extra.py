# Tests constants of the math module available only with MICROPY_PY_MATH_CONSTANTS.
try:
    import math
    from math import isnan

    math.tau
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

print(math.tau == 2.0 * math.pi)
print(math.copysign(1.0, math.tau))

print(math.inf == float("inf"))
print(-math.inf == -float("inf"))
print(math.copysign(1.0, math.inf))

print(isnan(math.nan))
print(isnan(-math.nan))
print(math.copysign(1.0, math.nan))
