# Tests domain errors in math functions

try:
    import math
except ImportError:
    print("SKIP")
    raise SystemExit

inf = float("inf")
nan = float("nan")

# single argument functions
for name, f in (
    ("fabs", math.fabs),
    ("ceil", math.ceil),
    ("floor", math.floor),
    ("trunc", math.trunc),
    ("sqrt", math.sqrt),
    ("exp", math.exp),
    ("log", math.log),
    ("sin", math.sin),
    ("cos", math.cos),
    ("tan", math.tan),
    ("asin", math.asin),
    ("acos", math.acos),
    ("atan", math.atan),
    ("ldexp", lambda x: math.ldexp(x, 0)),
    ("radians", math.radians),
    ("degrees", math.degrees),
):
    for x in (0, 1, 1.12, -1, -1.12, inf, -inf, nan):
        try:
            ans = "%.4f" % f(x)
        except ValueError:
            ans = "ValueError"
        except OverflowError:
            ans = "OverflowError"
        print("%s(%.4f) = %s" % (name, x, ans))

# double argument functions
for name, f in (
    ("pow", math.pow),
    ("log", math.log),
    ("fmod", math.fmod),
    ("atan2", math.atan2),
    ("copysign", math.copysign),
):
    for x in (
        (0, 0),
        (0, 2),
        (0, -1),
        (1, 0),
        (1.2, 0),
        (-1, 0),
        (-1, 2),
        (-1, 2.3),
        (0, inf),
        (0.5, inf),
        (0.5, -inf),
        (0.9, inf),
        (0.9, -inf),
        (1.2, inf),
        (1.2, -inf),
        (-0.5, inf),
        (-0.5, -inf),
        (-0.9, inf),
        (-0.9, -inf),
        (-1.2, inf),
        (-1.2, -inf),
        (inf, 0),
        (inf, 1.2),
        (inf, -1.2),
        (inf, inf),
        (inf, -inf),
        (-inf, inf),
        (-inf, -inf),
        (0, nan),
        (nan, 0),
        (1, nan),
        (nan, 1),
        (inf, nan),
        (nan, inf),
        (nan, nan),
    ):
        try:
            ans = "%.4f" % f(*x)
        except ValueError:
            ans = "ValueError"
        except ZeroDivisionError:
            ans = "ZeroDivisionError"
        print("%s(%.4f, %.4f) = %s" % (name, x[0], x[1], ans))
