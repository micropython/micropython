# Tests domain errors in math functions

try:
    import math
except ImportError:
    print("SKIP")
    raise SystemExit

inf = float('inf')
nan = float('nan')

# single argument functions
for name, f, args in (
    ('fabs', math.fabs, ()),
    ('ceil', math.ceil, ()),
    ('floor', math.floor, ()),
    ('trunc', math.trunc, ()),
    ('sqrt', math.sqrt, (-1, 0)),
    ('exp', math.exp, ()),
    ('sin', math.sin, ()),
    ('cos', math.cos, ()),
    ('tan', math.tan, ()),
    ('asin', math.asin, (-1.1, 1, 1.1)),
    ('acos', math.acos, (-1.1, 1, 1.1)),
    ('atan', math.atan, ()),
    ('ldexp', lambda x: math.ldexp(x, 0), ()),
    ('radians', math.radians, ()),
    ('degrees', math.degrees, ()),
    ):
    for x in args + (inf, nan):
        try:
            ans = f(x)
            print('%.4f' % ans)
        except ValueError:
            print(name, 'ValueError')
        except OverflowError:
            print(name, 'OverflowError')

# double argument functions
for name, f, args in (
    ('pow', math.pow, ((0, 2), (-1, 2), (0, -1), (-1, 2.3))),
    ('fmod', math.fmod, ((1.2, inf), (1.2, 0), (inf, 1.2))),
    ('atan2', math.atan2, ((0, 0),)),
    ('copysign', math.copysign, ()),
    ):
    for x in args + ((0, inf), (inf, 0), (inf, inf), (inf, nan), (nan, inf), (nan, nan)):
        try:
            ans = f(*x)
            print('%.4f' % ans)
        except ValueError:
            print(name, 'ValueError')
