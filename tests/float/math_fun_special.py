# test the special functions imported from math

try:
    from math import *

    erf
except (ImportError, NameError):
    print("SKIP")
    raise SystemExit

test_values = [
    -8.0,
    -2.5,
    -1,
    -0.5,
    0.0,
    0.5,
    2.5,
    8.0,
]
pos_test_values = [
    0.001,
    0.1,
    0.5,
    1.0,
    1.5,
    10.0,
]

functions = [
    ("expm1", expm1, test_values),
    ("log2", log2, test_values),
    ("log10", log10, test_values),
    ("cosh", cosh, test_values),
    ("sinh", sinh, test_values),
    ("tanh", tanh, [-1e6, -100] + test_values + [100, 1e6]),
    ("acosh", acosh, [1.0, 5.0, 1.0]),
    ("asinh", asinh, test_values),
    ("atanh", atanh, [-0.99, -0.5, 0.0, 0.5, 0.99]),
    ("erf", erf, test_values),
    ("erfc", erfc, test_values),
    ("gamma", gamma, pos_test_values),
    ("lgamma", lgamma, pos_test_values + [50.0, 100.0]),
]

is_REPR_C = float("1.0000001") == float("1.0")

for function_name, function, test_vals in functions:
    for value in test_vals:
        try:
            ans = "{:.4g}".format(function(value))
        except ValueError as e:
            ans = str(e)
        # a tiny error in REPR_C value for 1.5204998778 causes a wrong rounded value
        if is_REPR_C and function_name == "erfc" and ans == "1.521":
            ans = "1.52"
        print("{}({:.4g}) = {}".format(function_name, value, ans))
