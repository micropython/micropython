# test the special functions imported from math

try:
    from math import *
except ImportError:
    print("SKIP")
    import sys
    sys.exit()

test_values = [-8., -2.5, -1, -0.5, 0.0, 0.5, 2.5, 8.,]
pos_test_values = [0.001, 0.1, 0.5, 1.0, 1.5, 10.,]

functions = [
    ('erf', erf, test_values),
    ('erfc', erfc, test_values),
    ('gamma', gamma, pos_test_values),
    ('lgamma', lgamma, pos_test_values + [50., 100.,]),
]

for function_name, function, test_vals in functions:
    print(function_name)
    for value in test_vals:
        print("{:.5g}".format(function(value)))
