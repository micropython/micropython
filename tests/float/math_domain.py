# Tests domain errors in math functions

try:
    import math
except ImportError:
    print("SKIP")
    raise SystemExit

inf = float("inf")
nan = float("nan")

# single argument functions
for name, f, args in (
    ("fabs", math.fabs, ()),
    ("ceil", math.ceil, ()),
    ("floor", math.floor, ()),
    ("trunc", math.trunc, ()),
    ("sqrt", math.sqrt, (-1, 0)),
    ("exp", math.exp, ()),
    ("log", math.log, ()),
    ("sin", math.sin, ()),
    ("cos", math.cos, ()),
    ("tan", math.tan, ()),
    ("asin", math.asin, (-1.1, 1, 1.1)),
    ("acos", math.acos, (-1.1, 1, 1.1)),
    ("atan", math.atan, ()),
    ("ldexp", lambda x: math.ldexp(x, 0), ()),
    ("radians", math.radians, ()),
    ("degrees", math.degrees, ()),
):
    for x in args + (inf, -inf, nan):
        try:
            ans = "%.4f" % f(x)
        except ValueError:
            ans = "ValueError"
        except OverflowError:
            ans = "OverflowError"
        print("%s(%.4f) = %s" % (name, x, ans))

# double argument functions
for name, f, args in (
    (
        "pow",
        math.pow,
        (
            (0, 2),
            (-1, 2),
            (0, -1),
            (-1, 2.3),
            (0.5, inf),
            (-0.5, inf),
            (0.5, -inf),
            (-0.5, -inf),
            (1.5, inf),
            (-1.5, inf),
            (1.5, -inf),
            (-1.5, -inf),
            (nan, 0),
            (1, nan),
        ),
    ),
    ("log", math.log, ()),
    ("fmod", math.fmod, ((1.2, inf), (1.2, -inf), (1.2, 0), (inf, 1.2))),
    ("atan2", math.atan2, ((0, 0), (-inf, inf), (-inf, -inf), (inf, -inf))),
    ("copysign", math.copysign, ()),
):
    for x in args + ((0, inf), (inf, 0), (inf, inf), (inf, nan), (nan, inf), (nan, nan)):
        try:
            ans = "%.4f" % f(*x)
        except ValueError:
            ans = "ValueError"
        print("%s(%.4f, %.4f) = %s" % (name, x[0], x[1], ans))
