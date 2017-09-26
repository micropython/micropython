# Tests domain errors in math functions

try:
    import math
except ImportError:
    print("SKIP")
    raise SystemExit

inf = float('inf')

# pow function
print(math.pow(0, 2))
print(math.pow(-1, 2))
try:
    math.pow(0, -1)
except ValueError:
    print('pow ValueError')
try:
    math.pow(-1, 2.3)
except ValueError:
    print('pow ValueError')

# fmod function
print(math.fmod(1.2, inf))
try:
    math.fmod(1.2, 0)
except ValueError:
    print('fmod ValueError')
try:
    math.fmod(inf, 1.2)
except ValueError:
    print('fmod ValueError')

# trig functions
try:
    math.sin(inf)
except ValueError:
    print('sin ValueError')
try:
    math.cos(inf)
except ValueError:
    print('cos ValueError')
try:
    math.tan(inf)
except ValueError:
    print('tan ValueError')

# inverse trig functions
try:
    math.asin(1.1)
except ValueError:
    print('asin ValueError')
try:
    math.acos(1.1)
except ValueError:
    print('acos ValueError')
try:
    math.asin(inf)
except ValueError:
    print('asin ValueError')
try:
    math.acos(inf)
except ValueError:
    print('acos ValueError')
