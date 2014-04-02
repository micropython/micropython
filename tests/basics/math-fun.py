# Tests the functions imported from math

from math import *

test_values = [-100., -1.23456, -1, -0.5, 0.0, 0.5, 1.23456, 100.]
p_test_values = [0.1, 0.5, 1.23456]
unit_range_test_values = [-1., -0.75, -0.5, -0.25, 0., 0.25, 0.5, 0.75, 1.]

functions = [('sqrt', sqrt, p_test_values),
             ('exp', exp, test_values),
             ('expm1', expm1, test_values),
             ('log', log, p_test_values),
             ('log2', log2, p_test_values),
             ('log10', log10, p_test_values),
             ('cosh', cosh, test_values),
             ('sinh', sinh, test_values),
             ('tanh', tanh, test_values),
             ('acosh', acosh, [1.0, 5.0, 1.0]),
             ('asinh', asinh, test_values),
             ('atanh', atanh, [-0.99, -0.5, 0.0, 0.5, 0.99]),
             ('cos', cos, test_values),
             ('sin', sin, test_values),
             ('tan', tan, test_values),
             ('acos', acos, unit_range_test_values),
             ('asin', asin, unit_range_test_values),
             ('atan', atan, test_values),
             ('ceil', ceil, test_values),
             ('fabs', fabs, test_values),
             ('floor', floor, test_values),
             #('frexp', frexp, test_values),
             ('trunc', trunc, test_values)
            ]

for function_name, function, test_vals in functions:
    print(function_name)
    for value in test_vals:
        print("{:.7g}".format(function(value)))

binary_functions = [('copysign', copysign, [(23., 42.), (-23., 42.), (23., -42.),
                                (-23., -42.), (1., 0.0), (1., -0.0)])
                   ]

for function_name, function, test_vals in binary_functions:
    print(function_name)
    for value1, value2 in test_vals:
        print("{:.7g}".format(function(value1, value2)))
