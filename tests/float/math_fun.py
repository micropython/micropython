# Tests the functions imported from math

try:
    from math import *
except ImportError:
    print("SKIP")
    raise SystemExit

test_values = [-100.0, -1.23456, -1, -0.5, 0.0, 0.5, 1.23456, 100.0]
test_values_small = [
    -10.0,
    -1.23456,
    -1,
    -0.5,
    0.0,
    0.5,
    1.23456,
    10.0,
]  # so we don't overflow 32-bit precision
unit_range_test_values = [-1.0, -0.75, -0.5, -0.25, 0.0, 0.25, 0.5, 0.75, 1.0]

functions = [
    ("sqrt", sqrt, test_values),
    ("exp", exp, test_values_small),
    ("log", log, test_values),
    ("cos", cos, test_values),
    ("sin", sin, test_values),
    ("tan", tan, test_values),
    ("acos", acos, unit_range_test_values),
    ("asin", asin, unit_range_test_values),
    ("atan", atan, test_values),
    ("ceil", ceil, test_values),
    ("fabs", fabs, test_values),
    ("floor", floor, test_values),
    ("trunc", trunc, test_values),
    ("radians", radians, test_values),
    ("degrees", degrees, test_values),
]

for function_name, function, test_vals in functions:
    for value in test_vals:
        try:
            ans = "{:.5g}".format(function(value))
        except ValueError as e:
            ans = str(e)
        print("{}({:.5g}) = {}".format(function_name, value, ans))

tuple_functions = [
    ("frexp", frexp, test_values),
    ("modf", modf, test_values),
]

for function_name, function, test_vals in tuple_functions:
    for value in test_vals:
        x, y = function(value)
        print("{}({:.5g}) = ({:.5g}, {:.5g})".format(function_name, value, x, y))

binary_functions = [
    (
        "copysign",
        copysign,
        [(23.0, 42.0), (-23.0, 42.0), (23.0, -42.0), (-23.0, -42.0), (1.0, 0.0), (1.0, -0.0)],
    ),
    ("pow", pow, ((1.0, 0.0), (0.0, 1.0), (2.0, 0.5), (-3.0, 5.0), (-3.0, -4.0))),
    ("atan2", atan2, ((1.0, 0.0), (0.0, 1.0), (2.0, 0.5), (-3.0, 5.0), (-3.0, -4.0))),
    ("fmod", fmod, ((1.0, 1.0), (0.0, 1.0), (2.0, 0.5), (-3.0, 5.0), (-3.0, -4.0))),
    ("ldexp", ldexp, ((1.0, 0), (0.0, 1), (2.0, 2), (3.0, -2), (-3.0, -4))),
    (
        "log",
        log,
        (
            (2.0, 2.0),
            (3.0, 2.0),
            (4.0, 5.0),
            (0.0, 1.0),
            (1.0, 0.0),
            (-1.0, 1.0),
            (1.0, -1.0),
            (2.0, 1.0),
        ),
    ),
]

for function_name, function, test_vals in binary_functions:
    for value1, value2 in test_vals:
        try:
            ans = "{:.5g}".format(function(value1, value2))
        except (ValueError, ZeroDivisionError) as e:
            ans = type(e)
        print("{}({:.5g}, {:.5g}) = {}".format(function_name, value1, value2, ans))
